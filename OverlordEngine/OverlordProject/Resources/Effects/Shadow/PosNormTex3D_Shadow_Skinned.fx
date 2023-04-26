float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;
float4x4 gBones[70];
float2 gResolution = float2(50, 50);
float gAmbientLight = 0.35f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
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
	float4 lPos : TEXCOORD1;
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.texCoord = input.texCoord;
	
	// Declarations
	float4 transformedPosition = 0;
	float3 transformedNormal = 0;
	
	// Loop over bones
	for (int i = 0; i < 4; ++i)
    {
        float boneIndex = input.BoneIndices[i];
        transformedPosition += mul(float4(input.pos, 1.0) * input.BoneWeights[i], (float4x4) gBones[boneIndex]);
		transformedNormal += mul(input.normal * input.BoneWeights[i], (float3x3) gBones[boneIndex]);
    }
	
	// Ensure that w is 1
	transformedPosition.w = 1;
	
	// Transform to Clipping & World Space
	output.pos = mul( transformedPosition, gWorldViewProj );
	output.normal = normalize(mul(transformedNormal, (float3x3)gWorld));
	
	//store worldspace projected to light clip space with
    //a texcoord semantic to be interpolated across the surface
	output.lPos = mul( transformedPosition, mul( gWorld, gWorldViewProj_Light ) );
	
	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	u *= 1.f / gResolution.x;
	v *= 1.f / gResolution.y;
	return float2(u,v);
}

float EvaluateShadowMap(float4 lPos)
{
	//re-homogenize position after interpolation
    lPos.xyz /= lPos.w;
 
    //if position is not visible to the light - dont illuminate it
    //results in hard light frustum
    if( lPos.x < -1.0f || lPos.x > 1.0f ||
        lPos.y < -1.0f || lPos.y > 1.0f ||
        lPos.z < 0.0f  || lPos.z > 1.0f ) return 1.0f;
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lPos.x = lPos.x/2 + 0.5;
    lPos.y = lPos.y/-2 + 0.5;
 
	//apply shadow map bias
    lPos.z -= gShadowMapBias;
	
	//PCF sampling for shadow map
    float sum = 0;
    float x, y;
 
    //perform PCF filtering on a 4 x 4 texel neighborhood
    for (y = -1.5; y <= 1.5; y += 1.0)
    {
        for (x = -1.5; x <= 1.5; x += 1.0)
        {
            sum += gShadowMap.SampleCmpLevelZero( cmpSampler, lPos.xy + texOffset(x,y), lPos.z );
        }
    }
 
	return sum / 16.0;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb * shadowValue, color_a );
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

