float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

float4x4 gBones[125];

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 blendIndex : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHTS;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
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

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};

// ---------SHADOWMAP VARIABLES--------------
bool gEnableShadows = true;
static const int gMaxLights = 20;
int gAmountLights;
float4 gLightPosition[gMaxLights];
TextureCube gShadowCubeMap[gMaxLights];
float gNearPlanes[gMaxLights];
float gFarPlanes[gMaxLights];
float gAmbientLight = 0.25f;
float gShadowMapBias = 0.01f;
float gPCFsamples[gMaxLights];

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


//--------------------------------------------------------------------------------------
// Shadowmap functions
//--------------------------------------------------------------------------------------


float3 sampleOffsetDirections[21] = 
{
   float3( 0,  0,  0), float3( 1,  1,  1), float3( 1, -1,  1), float3(-1, -1,  1),
   float3(-1,  1,  1), float3( 1,  1, -1), float3( 1, -1, -1), float3(-1, -1, -1), 
   float3(-1,  1, -1), float3( 1,  1,  0), float3( 1, -1,  0), float3(-1, -1,  0), 
   float3(-1,  1,  0), float3( 1,  0,  1), float3(-1,  0,  1), float3( 1,  0, -1), 
   float3(-1,  0, -1), float3( 0,  1,  1), float3( 0, -1,  1), float3( 0, -1, -1), 
   float3( 0,  1, -1)
};

float EvaluateShadowMap(VS_OUTPUT input, int lightNumber)
{
    float3 fragToLight = input.worldPos.xyz - gLightPosition[lightNumber].xyz;
    float currentDepth = length(fragToLight);
    float normalizedDepth = (currentDepth - gNearPlanes[lightNumber]) / (gFarPlanes[lightNumber] - gNearPlanes[lightNumber]);
    normalizedDepth -= gShadowMapBias * normalizedDepth * normalizedDepth;

    float3 viewDirection = normalize(fragToLight);

    float viewDistance = length(fragToLight);

    float diskRadius = (1.0 + (viewDistance / gFarPlanes[lightNumber])) / 850.0;

    float shadow = 0.0;
    int samples = gPCFsamples[lightNumber];
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = gShadowCubeMap[lightNumber].Sample(cubeSampler, viewDirection + sampleOffsetDirections[i] * diskRadius).r;
        if(normalizedDepth < closestDepth)
            shadow += 1.0 - normalizedDepth;
    }
    shadow /= float(samples);

    return shadow;
}

float CalculateShadowValue(VS_OUTPUT input)
{
    float totalLighting = 0.f;

    for(int light = 0; light < gAmountLights; ++light)
    {
        totalLighting += EvaluateShadowMap(input, light);
    }
    
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
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;

	float4 transformedPosition = 0;
	float3 transformedNormal = 0;

	float4 originalPosition = float4(input.pos, 1);

	for(int i = 0; i < 4; ++i)
	{
		float boneIndex = input.blendIndex[i];
		if(boneIndex >= 0)
		{
			transformedPosition += 	input.blendWeight[i] * mul(originalPosition, gBones[boneIndex]); 
			transformedNormal 	+=	input.blendWeight[i] * mul(input.normal, (float3x3)gBones[boneIndex]); 
		}
	}
	transformedPosition.w = 1;

	// Step 1:	convert position into float4 and multiply with matWorldViewProj

	output.pos = mul(transformedPosition, gWorldViewProj);

	// Step 2:	rotate the normal: NO TRANSLATION
	//			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	//			thus removing the postion row of the matrix
	output.normal = normalize(mul(transformedNormal, (float3x3)gWorld));
	output.texCoord = input.texCoord;
    output.worldPos = mul( transformedPosition, gWorld );

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{

	float shadowValue = CalculateShadowValue(input);
	if(!gEnableShadows) shadowValue = 1.f;
	float diffuseStrength = CalculateHalfLambertDiffuse(input);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
    color_rgb *=  shadowValue * diffuseStrength;


	return float4( color_rgb , color_a );
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

