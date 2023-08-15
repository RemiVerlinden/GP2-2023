float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

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
	DepthEnable = FALSE;
	DepthWriteMask = NONE;
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
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 screenPos = input.screenPos;
	float2 uv;
	uv.x = screenPos.x / screenPos.w / 2.0f + 0.5f;
	uv.y = -screenPos.y / screenPos.w / 2.0f + 0.5f;

	float4 diffuseColor = gPortalMap.Sample( samLinear,uv );
	float3 color_rgb = diffuseColor.rgb;
	float color_a = 1;
	
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

