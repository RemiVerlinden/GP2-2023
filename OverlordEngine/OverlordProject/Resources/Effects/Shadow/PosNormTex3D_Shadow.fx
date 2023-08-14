float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

bool gEnableShadows = false;

static const int gMaxLights = 20;
int gAmountLights;
float4 gLightPosition[gMaxLights];
float gShadowMapBias = 0.005f;
float gShadowMapBiasPerspectiveMultiplier = 100;
float gNearPlane;
float gFarPlane;
float gAmbientLight = 0.35f;

Texture2D gDiffuseMap;
TextureCube gShadowCubeMap[gMaxLights];

SamplerComparisonState cmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;
 
   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};

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


SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
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
	CullMode = NONE;
};

//-------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
    output.normal = input.normal;
	output.texCoord = input.texCoord;

	//TODO: complete Vertex Shader
	//Hint: Don't forget to project our position to light clip space and store it in lPos
	float4 inputPos = float4(input.pos,1.0f);
    output.pos = mul( inputPos,  gWorldViewProj );
 
    //store worldspace projected to light clip space with
    //a texcoord semantic to be interpolated across the surface
    output.worldPos = mul( inputPos, gWorld );
    return output;
}

float EvaluateShadowMap(VS_OUTPUT input, int lightNumber)
{
	float3 cubeSampleDirection = input.worldPos.xyz - gLightPosition[lightNumber].xyz;
	float depth = length(cubeSampleDirection);
	float normalizedDepth = (depth - gNearPlane) / (gFarPlane - gNearPlane);
	normalizedDepth -= gShadowMapBias * normalizedDepth * normalizedDepth;

	cubeSampleDirection = normalize(cubeSampleDirection);
	float shadowValue = gShadowCubeMap[lightNumber].SampleCmpLevelZero(cubeCmpSampler, cubeSampleDirection, normalizedDepth);
	return shadowValue;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = 0.f;
	for(int light = 0; light < gAmountLights; ++light)
	{
		shadowValue += EvaluateShadowMap(input, light);
	}
	shadowValue /= gAmountLights;

	if(shadowValue < gAmbientLight) shadowValue = gAmbientLight;

   float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb = diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
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

color_rgb = color_rgb * avgDiffuseStrength;


	return float4( color_rgb * shadowValue , color_a );
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

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}