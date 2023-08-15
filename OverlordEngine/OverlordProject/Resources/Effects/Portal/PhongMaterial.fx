// #define FLIP_TEXTURE_Y


float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldInverseTranspose : WORLDINVERSETRANSPOSE;
float4x4 gWorld : WORLD;
float4x4 gViewInverse : VIEWINVERSE;

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
	float3 pos		    :POSITION;
	float2 texCoord		: TEXCOORD0;
	float3 normal		: NORMAL;
	float3 binormal		: BINORMAL;
	float3 tangent		: TANGENT;
};

struct VertexOutput
{
        float4 pos    		    : SV_POSITION;
	    float3 normal		    : NORMAL;
		float2 texCoord    		: TEXCOORD0;
		float3 lightVec   		: TEXCOORD1;
        float3 WorldNormal	    : TEXCOORD2;
        float3 WorldTangent	    : TEXCOORD3;
        float3 WorldBinormal    : TEXCOORD4;
        float3 WorldView	    : TEXCOORD5;
	    float4 worldPos         : TEXCOORD6;
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



//--------------------------------------------------------------------------------------
// Shadowmap functions
//--------------------------------------------------------------------------------------

// ---------SHADOWMAP VARIABLES--------------
bool gEnableShadows = true;
static const int gMaxLights = 20;
int gAmountLights;
float4 gLightPosition[gMaxLights];
TextureCube gShadowCubeMap[gMaxLights];
float gNearPlanes[20];
float gFarPlanes[20];
float gAmbientLight = 0.45f;
float gShadowMapBias = 0.01f;
float gPCFsamples = 20;

SamplerState cubeSampler
{
    Filter = MIN_MAG_MIP_LINEAR;   // Tri-linear interpolation
    AddressU = CLAMP;              // Clamp the texture coordinates to the valid range
    AddressV = CLAMP;
    AddressW = CLAMP;
    ComparisonFunc = LESS;        // For shadow mapping, might be useful to add
    BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
};


//-------------------------------------------

// we can use a max of 21 in gPCFsamples but for optimisation we will most likely use less
float3 sampleOffsetDirections[21] = 
{
   float3( 0,  0,  0), float3( 1,  1,  1), float3( 1, -1,  1), float3(-1, -1,  1),
   float3(-1,  1,  1), float3( 1,  1, -1), float3( 1, -1, -1), float3(-1, -1, -1), 
   float3(-1,  1, -1), float3( 1,  1,  0), float3( 1, -1,  0), float3(-1, -1,  0), 
   float3(-1,  1,  0), float3( 1,  0,  1), float3(-1,  0,  1), float3( 1,  0, -1), 
   float3(-1,  0, -1), float3( 0,  1,  1), float3( 0, -1,  1), float3( 0, -1, -1), 
   float3( 0,  1, -1)
};

float EvaluateShadowMap(VertexOutput input, int lightNumber)
{
    float3 fragToLight = input.worldPos.xyz - gLightPosition[lightNumber].xyz;
    float currentDepth = length(fragToLight);
    float normalizedDepth = (currentDepth - gNearPlanes[lightNumber]) / (gFarPlanes[lightNumber] - gNearPlanes[lightNumber]);
    normalizedDepth -= gShadowMapBias * normalizedDepth * normalizedDepth;

    float3 viewDirection = normalize(fragToLight);

    float viewDistance = length(fragToLight);

    float diskRadius = (1.0 + (viewDistance / gFarPlanes[lightNumber])) / 850.0;

    float shadow = 0.0;
    int samples = gPCFsamples;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = gShadowCubeMap[lightNumber].Sample(cubeSampler, viewDirection + sampleOffsetDirections[i] * diskRadius).r;
        if(normalizedDepth < closestDepth)
            shadow += 1.0 - normalizedDepth;
    }
    shadow /= float(samples);

    return shadow;
}

float CalculateShadowValue(VertexOutput input)
{
    float totalLighting = 0.f;

    for(int light = 0; light < gAmountLights; ++light)
    {
        totalLighting += EvaluateShadowMap(input, light);
    }
    
    totalLighting = clamp(totalLighting, gAmbientLight, 1.0f);
    return totalLighting;
}

float CalculateHalfLambertDiffuse(VertexOutput input)
{
	float3 accumulatedLight = float3(0, 0, 0);

	for (int i = 0; i < gAmountLights; ++i)
	{
    	float3 lightDirection = normalize(input.worldPos.xyz - gLightPosition[i]);
    	float lightDiffuseStrength = dot(input.normal, -lightDirection);
    	lightDiffuseStrength = lightDiffuseStrength * 0.5 + 0.5;
    	lightDiffuseStrength = saturate(lightDiffuseStrength);
    
    	accumulatedLight += lightDiffuseStrength;
	}

	// Average out the diffuse strength.
	float avgDiffuseStrength = accumulatedLight / gAmountLights;
	return avgDiffuseStrength;
}


/**************************************/
/***** VERTEX SHADER ******************/
/**************************************/

VertexOutput VS(VertexInput IN) {	
    VertexOutput OUT = (VertexOutput)0;
    OUT.WorldNormal = mul(float4(IN.normal,1.0),gWorldInverseTranspose).xyz;
    OUT.WorldTangent = mul(float4(IN.tangent,1.0),gWorldInverseTranspose).xyz;
    OUT.WorldBinormal = mul(float4(IN.binormal,1.0),gWorldInverseTranspose).xyz;
    float4 Po = float4(IN.pos.xyz,1);
    float3 Pw = mul(Po,gWorld).xyz;
    OUT.lightVec = -normalize(gLightDirection);
#ifdef FLIP_TEXTURE_Y
    OUT.texCoord = float2(IN.texCoord.x,(1.0-IN.texCoord.y));
#else /* !FLIP_TEXTURE_Y */
    OUT.texCoord = IN.texCoord.xy;
#endif /* !FLIP_TEXTURE_Y */
    OUT.WorldView = normalize(gViewInverse[3].xyz - Pw);
    OUT.pos = mul(Po,gWorldViewProj);

    OUT.normal = IN.normal;
    OUT.worldPos = mul( float4(IN.pos,1.0f), gWorld );

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

    float shadowValue = CalculateShadowValue(IN);
	float diffuseStrength = CalculateHalfLambertDiffuse(IN);

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
    result = diffuseColor * shadowValue * diffuseStrength;

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

