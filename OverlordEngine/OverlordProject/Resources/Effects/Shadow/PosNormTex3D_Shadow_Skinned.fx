float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
static const int gMaxLights = 20;
int gAmountLights;
float4 gLightPosition[gMaxLights];
float gNearPlane;
float gFarPlane;
float gShadowMapBias = 0.005f;
float gShadowMapBiasPerspectiveMultiplier = 100;
float gAmbientLight = 0.35f;
bool gEnableShadows = false;

float4x4 gBones[125];

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
 	AddressW = Wrap;// or Mirror or Clamp or Border
};


struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 BoneIndices : BLENDINDICES;
	float4 BoneWeights : BLENDWEIGHTS;
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
// Helper Functions
//--------------------------------------------------------------------------------------

// Constants for the frustums
#define POS_X 0
#define NEG_X 1
#define POS_Y 2
#define NEG_Y 3
#define POS_Z 4
#define NEG_Z 5

int pointInFrustum(float3 lightPos, float3 fragPos)
{
	float3 dir = normalize(fragPos - lightPos);

    float maxX = abs(dir.x);
    float maxY = abs(dir.y);
    float maxZ = abs(dir.z);

    if(maxX > maxY && maxX > maxZ)
    {
        return dir.x > 0.0f ? POS_X : NEG_X;
    }
    else if(maxY > maxX && maxY > maxZ)
    {
        return dir.y > 0.0f ? POS_Y : NEG_Y;
    }
    return dir.z > 0.0f ? POS_Z : NEG_Z;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//TODO: complete Vertex Shader 
	//Hint: use the previously made shaders PosNormTex3D_Shadow and PosNormTex3D_Skinned as a guide
	float4 originalPosition = float4(input.pos, 1.0f);
	float4 transformedPosition = 0;
	float3 transformedNormal = 0;
	float index;
	for (float i = 0; i < 4; ++i)
	{
		index = input.BoneIndices[i];
		if (index > -1)
		{
			transformedPosition += input.BoneWeights[i] * mul(originalPosition, gBones[index]);
			transformedNormal += input.BoneWeights[i] * mul(input.normal, (float3x3)gBones[index]);
		}
	}
	transformedPosition.w = 1;

	//TODO: complete Vertex Shader 
	//Hint: use the previously made shaders PosNormTex3D_Shadow and PosNormTex3D_Skinned as a guide
	output.pos = mul(transformedPosition, gWorldViewProj);
	output.normal = transformedNormal;
	output.texCoord = input.texCoord;
	output.worldPos = mul(transformedPosition, gWorld);

	return output;
}

float EvaluateShadowMap(VS_OUTPUT input, int lightNumber)
{
	float3 cubeSampleDirection = input.worldPos.xyz - gLightPosition[lightNumber].xyz;
	float depth = length(cubeSampleDirection);
	float normalizedDepth = (depth - gNearPlane) / (gFarPlane - gNearPlane);
	normalizedDepth -= gShadowMapBias;

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




// float EvaluateShadowMap(float4 lpos, float3 normal)
// {
// 	//re-homogenize position after interpolation
//     lpos.xyz /= lpos.w;

// 	//if position is not visible to the light - dont illuminate it
//     //results in hard light frustum
// 	if (lpos.x < -1.0f || lpos.x > 1.0f ||
// 		lpos.y < -1.0f || lpos.y > 1.0f ||
// 		lpos.z < 0.0f || lpos.z > 1.0f)
// 	{		
// 		return 1.0f;
// 	}
	
// 	//transform clip space coords to texture space coords (-1:1 to 0:1)
// 	lpos.x = lpos.x * 0.5f + 0.5f;
// 	lpos.y = lpos.y * -0.5f + 0.5f;
	
// 	//apply shadow map bias

// 	lpos.z -= gShadowMapBias / (lpos.z*gShadowMapBiasPerspectiveMultiplier); // if set to 0 while shadow map is in perspective, I get perfect shadows(with acne)
	
// 	float shadowValue = gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy, lpos.z);
// 	return shadowValue;
// }