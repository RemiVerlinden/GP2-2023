float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

float gTime = 0.f;
float2 gResolution = float2(1920,1080);

Texture2D gPortalMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT{
	float3 pos : POSITION;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 screenPos : TEXCOORD1;
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
	
	output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	output.screenPos = output.pos;
	
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float snoise(float3 uv, float res)
{
	float3 s = float3(1e0, 1e2, 1e3);
	
	uv *= res;
	
	float3 uv0 = floor(fmod(uv, res))*s;
	float3 uv1 = floor(fmod(uv+float3(1.0, 1.0, 1.0), res))*s;
	
	float3 f = frac(uv); f = f*f*(3.0-2.0*f);

	float4 v = float4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	float4 r = frac(sin(v*1e-1)*1e3);
	float r0 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);
	
	r = frac(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);
	
	return lerp(r0, r1, f.z)*2.-1.;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 screenPos = input.screenPos;
	float2 uv;
	
	uv.x = screenPos.x / screenPos.w / 2.0f + 0.5f;
	uv.y = -screenPos.y / screenPos.w / 2.0f + 0.5f;

	float4 diffuseColor = gPortalMap.Sample( samLinear,uv );

	// added code
	float2 p = -.5 + uv;
	p.x *= gResolution.x/gResolution.y;
	
	float color = 3.0 - (3.*length(2.*p));
	
	float3 coord = float3(atan2(p.y,p.x)/6.2832+.5, length(p)*.4, .5);
	
	for(int i = 1; i <= 7; i++)
	{
		float power = pow(2.0, float(i));
		color += (1.5 / power) * snoise(coord + float3(0.,-gTime*.05, gTime*.01), power*16.);
	}
	
	float4 noiseColor = float4( color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.)*0.15 , 1.0);
	
	// composite noiseColor on top of diffuseColor
	return float4( lerp(diffuseColor.rgb, noiseColor.rgb, noiseColor.a), 1.f);
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

