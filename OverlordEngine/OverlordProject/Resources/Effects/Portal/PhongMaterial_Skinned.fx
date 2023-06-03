// #define FLIP_TEXTURE_Y

float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldInverseTranspose : WORLDINVERSETRANSPOSE;
float4x4 gWorld : WORLD;
float4x4 gViewInverse : VIEWINVERSE;

float4x4 gBones[70];

/************** light info **************/
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

float4 light1Color = float4( 1.0f, 1.0f, 1.0f, 1.0f );


/************* TWEAKABLES **************/
float3 gAmbientColor = {0.07f, 0.07f, 0.07f};
float4 gDiffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
float gSpecularPower = 55.0;
float gSpecular = 0.5;
float gNormal = 1.0;


bool gUseTextureNormal = false;
bool gUseTextureDiffuse = false;

Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;


/****************************************************/
/********** SAMPLER ********************************/
/****************************************************/

SamplerState TextureSampler
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 8; // Adjust the value based on your requirements
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};


/****************************************************/
/********** CG SHADER FUNCTIONS *********************/
/****************************************************/

struct VertexInput
{
	float3 position		: POSITION;
	float2 texCoord		: TEXCOORD0;
	float3 normal		: NORMAL;
	float3 binormal		: BINORMAL;
	float3 tangent		: TANGENT;
    float4 blendIndex : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHTS;
};

struct VertexOutput
{
        float4 position    		: SV_POSITION;
		float2 texCoord    		: TEXCOORD0;
		float3 lightVec   		: TEXCOORD1;
        float3 WorldNormal	    : TEXCOORD2;
        float3 WorldTangent	    : TEXCOORD3;
        float3 WorldBinormal    : TEXCOORD4;
        float3 WorldView	    : TEXCOORD5;
};


/****************************************************/
/******************** STATES ************************/
/****************************************************/

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = FRONT;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};


/**************************************/
/***** VERTEX SHADER ******************/
/**************************************/

VertexOutput VS(VertexInput IN) 
{	
    VertexOutput OUT = (VertexOutput)0;

    // Bone transformations
    float4 transformedPosition = 0;
    float3 transformedNormal = 0;
    float3 transformedBinormal = 0;
    float3 transformedTangent = 0;

    for (int i = 0; i < 4; ++i)
    {
        if (IN.blendIndex[i] >= 0)
        {
            transformedPosition += IN.blendWeight[i] * mul(float4(IN.position, 1.0), gBones[IN.blendIndex[i]]);
            transformedNormal += IN.blendWeight[i] * mul(IN.normal, (float3x3)gBones[IN.blendIndex[i]]);
            transformedBinormal += IN.blendWeight[i] * mul(IN.binormal, (float3x3)gBones[IN.blendIndex[i]]);
            transformedTangent += IN.blendWeight[i] * mul(IN.tangent, (float3x3)gBones[IN.blendIndex[i]]);
        }
    }

    transformedPosition.w = 1;
    
    // Rest of the vertex shader...
    float3 Pw = transformedPosition.xyz;
    OUT.lightVec = -normalize(gLightDirection);
#ifdef FLIP_TEXTURE_Y
    OUT.texCoord = float2(IN.texCoord.x, (1.0 - IN.texCoord.y));
#else
    OUT.texCoord = IN.texCoord.xy;
#endif
    OUT.WorldView = normalize(gViewInverse[3].xyz - Pw);
    OUT.position = mul(transformedPosition, gWorldViewProj);

    OUT.WorldNormal = normalize(transformedNormal);
    OUT.WorldTangent = normalize(transformedTangent);
    OUT.WorldBinormal = normalize(transformedBinormal);

    return OUT;
}


/**************************************/
/***** FRAGMENT PROGRAM ***************/
/**************************************/

void phong_shading(VertexOutput IN,
		    float3 LightColor,
		    float3 Nn,
		    float3 Ln,
		    float3 Vn,
		    out float3 DiffuseContrib,
		    out float3 SpecularContrib)
{
    float3 Hn = normalize(Vn + Ln);
    float4 litV = lit(dot(Ln,Nn),dot(Hn,Nn),gSpecularPower);
    DiffuseContrib = litV.y * LightColor;
    SpecularContrib = litV.y * litV.z * gSpecular * LightColor;
}


float4 PS(VertexOutput IN) : SV_TARGET {
    float3 diffContrib;
    float3 specContrib;

    // make everything correct for calulation
    float3 Ln = normalize(IN.lightVec);
    float3 Vn = normalize(IN.WorldView);
    float3 Nn = normalize(IN.WorldNormal);
    float3 Tn = normalize(IN.WorldTangent);
    float3 Bn = normalize(IN.WorldBinormal);
    
    // get normal map if present
    if(gUseTextureNormal)
    { 
        float3 bump = gNormal * (gNormalMap.Sample(TextureSampler, IN.texCoord).rgb - float3(0.5, 0.5, 0.5));
        Nn = Nn + bump.x * Tn + bump.y * Bn;
        Nn = normalize(Nn);
    }
    phong_shading(IN, light1Color, Nn, Ln, Vn, diffContrib, specContrib);

    // diffuse
    float3 diffuseColor = gDiffuseColor;
    if(gUseTextureDiffuse) diffuseColor = gDiffuseMap.Sample(TextureSampler, IN.texCoord).rgb; 

    // final combine
    float3 result = specContrib + (diffuseColor * (diffContrib + gAmbientColor));

    return float4(result, 1.0);
}

/****************************************************/
/********** TECHNIQUES ******************************/
/****************************************************/

technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

