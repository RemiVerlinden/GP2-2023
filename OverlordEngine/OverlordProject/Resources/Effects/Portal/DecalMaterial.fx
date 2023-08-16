float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 


Texture2D gDecalTexture;
float gAlphaCutoff = 0.1f; // any alpha value that is smaller than this one will be clipped
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
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
	CullMode = NONE;
};

BlendState AlphaBlend
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
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
float gNearPlanes[gMaxLights];
float gFarPlanes[gMaxLights];
float gAmbientLight = 0.25f;
float gShadowMapBias = 0.01f;
float gPCFsamples[gMaxLights];


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
	output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	output.texCoord = input.texCoord;
	    output.worldPos = mul( float4(input.pos,1.0f), gWorld );
    output.normal = input.normal;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float shadowValue = CalculateShadowValue(input);
	float diffuseStrength = CalculateHalfLambertDiffuse(input);
	
	float4 decalColor = gDecalTexture.Sample( samLinear,input.texCoord );
	clip(decalColor.a - gAlphaCutoff);

	decalColor.rgb *= shadowValue * diffuseStrength;

	return decalColor;
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
		SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

