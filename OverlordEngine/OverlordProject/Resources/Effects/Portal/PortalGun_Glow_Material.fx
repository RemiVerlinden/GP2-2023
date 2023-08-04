float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gTime;

float3 gColor = float3(1,1,1);


float4x4 gBones[125];

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
	float2 uv : TEXCOORD1;
	float4 blendIndex : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHTS;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD1;
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
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	output. uv = input.uv;

	float4 transformedPosition = 0;

	float4 originalPosition = float4(input.pos, 1);

	for(int i = 0; i < 4; ++i)
	{
		float boneIndex = input.blendIndex[i];
		if(boneIndex >= 0)
		{
			transformedPosition += 	input.blendWeight[i] * mul(originalPosition, gBones[boneIndex]); 
		}
	}
	transformedPosition.w = 1;

	output.pos = mul(transformedPosition, gWorldViewProj);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // Convert position to range from -1 to 1
    float2 uvCoord = float2(input.uv.x * 2.0 - 1.0, input.uv.y * 2.0 - 1.0);
    
    float timeOffset = gTime;

    // Recalculate positions with time influence
    float movA = uvCoord.x + uvCoord.y + 100.0 * cos(2.0 * sin(timeOffset)) + 1000.0 * sin(uvCoord.x / 100.0);
    float movB = uvCoord.y / 0.9 + timeOffset;
    float movC = uvCoord.x / 0.2;

    // Generate color components with complex sine and cosine operations
    float colorA = abs((sin(movB + timeOffset) / 2.0) + (movC / 2.0) - movB - movC + timeOffset);
    float colorB = abs(sin(colorA + sin(movA / 1000.0 + timeOffset) + sin(uvCoord.y / 40.0 + timeOffset) + sin((uvCoord.x + uvCoord.y) / 100.0) * 3.0));
    float colorC = abs(sin(colorB + cos(movB + movC + colorB) + cos(movC) + sin(uvCoord.x / 1000.0)));

    // Combine color components into a single color
    float3 transformedColor = float3(colorA, colorB, colorC);
    
    // Lerp between gColor and outputColor
    float t = 0.3; // Adjust this value to control the amount of each color. 0.0 = all gColor, 1.0 = all outputColor
    transformedColor = lerp(gColor, transformedColor, t);

    return float4(transformedColor, 1.0);
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

// https://www.shadertoy.com/view/ldBGRR