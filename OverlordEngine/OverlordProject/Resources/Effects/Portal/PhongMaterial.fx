float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorld : WORLD;
float4x4 gViewInverse : VIEWINVERSE;


/************** light info **************/
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

float4 light1Color = float4( 1.0f, 1.0f, 1.0f, 1.0f );


/************* TWEAKABLES **************/
float4 AmbientColor = {0.25f, 0.25f, 0.25f, 1.0f};
float4 DiffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
float4 SpecularColor = {1.0f, 1.0f, 1.0f, 1.0f};
float Glossiness = 20.0;


Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;
bool gUseSpecular = false;


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
};

struct VertexOutput
{
        float4 position    		: SV_POSITION;
		float2 texCoord    		: TEXCOORD0;
		float3 lightVec   		: TEXCOORD1;
		float3 eyeVec   		: TEXCOORD2;
		float3 worldNormal		: TEXCOORD3;
		float3 worldBinormal	: TEXCOORD4;
		float3 worldTangent		: TEXCOORD5;
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


/****************************************************/
/******************** HELPER ************************/
/****************************************************/

float4x4 CalculateInverse(float4x4 m)
{
    float3 cofactors0 = cross(m[1].yzw, m[2].yzw);
    float3 cofactors1 = cross(m[2].yzw, m[0].yzw);
    float3 cofactors2 = cross(m[0].yzw, m[1].yzw);

    float3 signs = float3(1.0, -1.0, 1.0);

    float3x3 cofactorMatrix = float3x3(
        signs[0] * cofactors0,
        signs[1] * cofactors1,
        signs[2] * cofactors2
    );

    float determinant = dot(m[0].xyz, cofactors0);

    float invDeterminant = 1.0 / determinant;

    float4x4 inverseMatrix = float4x4(
        float4(cofactorMatrix[0] * invDeterminant, 0.0),
        float4(cofactorMatrix[1] * invDeterminant, 0.0),
        float4(cofactorMatrix[2] * invDeterminant, 0.0),
        float4(0.0, 0.0, 0.0, 1.0)
    );

    return transpose(inverseMatrix);
}


/**************************************/
/***** VERTEX SHADER ******************/
/**************************************/

VertexOutput VS(VertexInput In)
{
    float4x4 worldInverse = CalculateInverse(gWorld);
    float4x4 worldInverseTranspose = transpose(worldInverse);

	VertexOutput Out; 															//create the output struct
    Out.worldNormal = mul(float4(In.normal,1.0), worldInverseTranspose).xyz;		//put the normal in world space pass it to the pixel shader
    Out.worldBinormal = mul(float4(In.binormal,1.0), worldInverseTranspose).xyz;	//put the binormal in world space pass it to the pixel shader
    Out.worldTangent = mul(float4(In.tangent,1.0), worldInverseTranspose).xyz;		//put the tangent in world space pass it to the pixel shader
    float3 worldSpacePos = mul(float4(In.position,1.0), gWorld).xyz;				//put the vertex in world space
    // Out.lightVec = lightPosition - worldSpacePos;
    Out.lightVec = gLightDirection;					                    //create the world space light vector and pass it to the pixel shader
	Out.eyeVec = (float3)gViewInverse[3] - worldSpacePos; 						//create the eye vector in world space and pass it to the pixel shader
	Out.texCoord.xy = In.texCoord;										//pass the UV coordinates to the pixel shader
    Out.position = mul(float4(In.position,1.0), gWorldViewProj);				//put the vertex position in clip space and pass it to the pixel shader
    return Out;
}


/**************************************/
/***** FRAGMENT PROGRAM ***************/
/**************************************/

float4 PS(VertexOutput In,uniform float4 lightColor) : SV_TARGET
{
// Fetch the diffuse and normal maps
float4 ColorTexture = gDiffuseMap.Sample(TextureSampler, In.texCoord.xy);
float4 SpecularTexture = gSpecularMap.Sample(TextureSampler, In.texCoord.xy);
float3 normal = gNormalMap.Sample(TextureSampler, In.texCoord.xy).xyz * 2.0 - 1.0;

// Create tangent space vectors
float3 Nn = In.worldNormal;
float3 Bn = In.worldBinormal;
float3 Tn = In.worldTangent;

// Offset world space normal with normal map values
float3 N = (normal.z * Nn) + (normal.x * Bn) + (normal.y * -Tn);
N = normalize(N);

// Create lighting vectors - view vector and light vector
float3 L = normalize(In.lightVec.xyz);
float3 V = normalize(In.eyeVec.xyz);

// Lighting

// Ambient light
float4 Ambient = AmbientColor * ColorTexture;

// Diffuse light
float diffuselight = saturate(dot(Nn, -L));
float4 Diffuse = DiffuseColor * ColorTexture * diffuselight;

// Specular light
float4 Specular;
if(gUseSpecular)
{
    float3 R = -reflect(L, N);
    float RdotV = saturate(dot(R, V));
    float gloss = Glossiness * SpecularTexture.a;
    float SpecPower = pow(RdotV, gloss);
    Specular = SpecPower * SpecularColor * SpecularTexture;
}

return (Ambient + Diffuse + Specular) * lightColor;

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

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS(light1Color) ) );
    }
}

