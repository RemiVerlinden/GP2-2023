float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gTime = 0;
float gScrollSpeed = 0;
Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;  // or Mirror or Clamp or Border
    AddressV = Wrap;  // or Mirror or Clamp or Border
    MaxAnisotropy = 16;
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
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
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
// Noise Functions
//--------------------------------------------------------------------------------------
float3 mod289(float3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float2 mod289(float2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 permute(float3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(float2 v) 
{
const float4 C = float4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0)) + i.x + float3(0.0, i1.x, 1.0));

    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;

    float3 x = 2.0 * frac(p * C.wwww) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float staticV(float2 uv, float iTime) 
{
	float staticHeight = snoise(float2(9.0,iTime*1.2+3.0))*0.3+5.0;
    float staticAmount = snoise(float2(1.0,iTime*1.2-6.0))*0.1+0.3;
    float staticStrength = snoise(float2(-9.75,iTime*0.6-3.0))*2.0+2.0;
	return (1.0-step(snoise(float2(5.0*pow(iTime,2.0)+pow(uv.x*7.0,1.2),pow((fmod(iTime,100.0)+100.0)*uv.y*0.3+3.0,staticHeight))),staticAmount))*staticStrength;
}
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	// Step 2:	rotate the normal: NO TRANSLATION
	//			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	//			thus removing the postion row of the matrix
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	input.texCoord.y += gTime * gScrollSpeed;

	output.texCoord = input.texCoord;
	return output;
}

float gVertJerkOpt = 0.0;
float gVertMovementOpt = .1;

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float2 uv = input.texCoord;

    float jerkOffset = (1.0 - step(snoise(float2(gTime * 1.3, 5.0)), 0.8)) * 0.05;
    float fuzzOffset = snoise(float2(gTime * 15.0, uv.y * 80.0)) * 0.003;
    float largeFuzzOffset = snoise(float2(gTime * 1.0, uv.y * 25.0)) * 0.004;

    float vertMovementOn = (1.0 - step(snoise(float2(gTime * 0.2, 8.0)), 0.4)) * gVertMovementOpt;
    float vertJerk = (1.0 - step(snoise(float2(gTime * 1.5, 5.0)), 0.6)) * gVertJerkOpt;
    float vertJerk2 = (1.0 - step(snoise(float2(gTime * 5.5, 5.0)), 0.2)) * gVertJerkOpt;
    float yOffset = abs(sin(gTime) * 4.0) * vertMovementOn + vertJerk * vertJerk2 * 0.3;
    float y = fmod(uv.y + yOffset, 1.0);

    float xOffset = (fuzzOffset + largeFuzzOffset);
    
    float staticVal = 0.0;
    for (float vert = -1.0; vert <= 1.0; vert += 1.0) {
        float maxDist = 5.0 / 400.0;
        float dist = vert / 200.0;
        staticVal += staticV(float2(uv.x, uv.y + dist), gTime) * (maxDist - abs(dist)) * 1.5;
    }

    float red = gDiffuseMap.Sample(samLinear, float2(uv.x + xOffset - 0.01, y)).r + staticVal;
    float green = gDiffuseMap.Sample(samLinear, float2(uv.x + xOffset, y)).g + staticVal;
    float blue = gDiffuseMap.Sample(samLinear, float2(uv.x + xOffset + 0.01, y)).b + staticVal;

    float3 color = float3(red, green, blue);
    float scanline = sin(uv.y * 800.0) * 0.04;
    color -= scanline;

    return float4(color, 1.0);
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


// https://www.shadertoy.com/view/ldXGW4