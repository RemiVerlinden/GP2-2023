float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

// ---------SHADOWMAP VARIABLES--------------
bool gEnableShadows = true;
static const int gMaxLights = 20;
int gAmountLights;
float4 gLightPosition[gMaxLights];
TextureCube gShadowCubeMap[gMaxLights];
float gNearPlane;
float gFarPlane;
float gAmbientLight = 0.f;
float gShadowMapBias = 0.01f;

// SAMPLER
SamplerComparisonState cubeCmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;
   AddressW = MIRROR;
   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};

//-------------------------------------------

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 8; // Adjust the value based on your requirements
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 worldPos : TEXCOORD1;

};

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
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	output.texCoord = input.texCoord;

    output.worldPos = mul( float4(input.pos,1.0f), gWorld );
    output.normal = input.normal;

	return output;
}

//--------------------------------------------------------------------------------------
// Shadowmap functions
//--------------------------------------------------------------------------------------
float EvaluateShadowMap(VS_OUTPUT input, int lightNumber) // true means point is lit
{
	float3 cubeSampleDirection = input.worldPos.xyz - gLightPosition[lightNumber].xyz;
	float depth = length(cubeSampleDirection);
	float normalizedDepth = (depth - gNearPlane) / (gFarPlane - gNearPlane);
	normalizedDepth -= gShadowMapBias * normalizedDepth * normalizedDepth;

	cubeSampleDirection = normalize(cubeSampleDirection);
	bool isLit = gShadowCubeMap[lightNumber].SampleCmpLevelZero(cubeCmpSampler, cubeSampleDirection, normalizedDepth);
	float shadowValue = 0.f;
	if(isLit)
	{
		shadowValue = clamp( 1.0f - normalizedDepth,0.f,1.f);
	}


	return shadowValue;
}

float CalculateShadowValue(VS_OUTPUT input)
{
	float totalLighting = 0.f;

	for(int light = 0; light < gAmountLights; ++light)
	{
		totalLighting += EvaluateShadowMap(input, light);
	}
	
	// Clamp the lighting so it doesn't go above 1.0 (or some max value)
	// and doesn't go below the ambient light level.
	totalLighting = clamp(totalLighting, gAmbientLight, 1.0f);
	return totalLighting;
}

float CalculateHalfLambertDiffuse(VS_OUTPUT input)
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

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float shadowValue = CalculateShadowValue(input);
	float diffuseStrength = CalculateHalfLambertDiffuse(input);

	float3 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	if(gEnableShadows)
		diffuseColor *= shadowValue;

	return float4(diffuseColor ,1.f);
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

