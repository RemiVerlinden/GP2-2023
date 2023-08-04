float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

float gTime = 0.f;
float2 gResolution = float2(1920,1080);
bool gBluePortalColor = true;

Texture2D gPortalMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT{
	float3 pos : POSITION;
	float2 uv : TEXCOORD1;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD1;
	float4 screenPos : TEXCOORD2;

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
	output. uv = input.uv;
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
float gMaxClipRadius = 0.47f;
float gPortalOpenDuration = 0.4f;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 screenPos = input.screenPos;
	float2 uv = input.uv;
	
	screenPos.x = screenPos.x / screenPos.w / 2.0f + 0.5f;
	screenPos.y = -screenPos.y / screenPos.w / 2.0f + 0.5f;

	float4 diffuseColor = gPortalMap.Sample( samLinear,screenPos.xy );

	 
	// Calculate portal scale, it will go from 0 to 1 over a certain duration
    float portalScale = min(gTime, gPortalOpenDuration);

	// Calculate portal scale, it will go from 0 to 1 over a certain duration
    portalScale = smoothstep(0.0, gPortalOpenDuration, portalScale); // Apply easing function when opening portal

	float2 p = -.5 + uv;
	p /= portalScale;

	
	// I added the portal scale so during the portal open animation, the plasma edges will stay much bigger than otherwise( else when the portal is 0.1 scale, the plasma edge is also 0.1 in side and it looks a little dumb)
	// NOTE: whatever I am doing is very much NOT perfect but I am happy with how it looks and I need to move on and do different stuff 
	float color = 3.0 -  (3.*length(1.8*p) / portalScale); // this will calculate a range between [0-3] where 0 means no color and 3 means max color. -> everything closer to center is closer to 0 (max number - length of coordinate)
	
	float rotationSpeed = 15.f; // higher number means slower rotation 
	float temporalRotation = 2 + gTime / rotationSpeed; // there is a certain threshhold between [0 - something close to 1(maybe PI/2)] where get a seam on the side of the portal so to prevent this, I just start off with a higher value


	float3 coord = float3(atan2(p.y,p.x)/6.2832 + temporalRotation, length(p)*.8, gTime / 15.); // the Z of the coord can be static but I like it a little more when the Z part of the noise slide also moves a bit for more dramatic effect
	coord = 1. - coord;
	
	for(int i = 1; i <= 2; i++)
	{
		float power = pow(2., float(i));
		color += (0.4 / power) * snoise(coord + float3(0.,-gTime*.07, gTime*.015), power*8.);
	}
	
    color = 1.0 - color;
    color *= 2.7 * pow(portalScale,2); // must have this in conjunction with the [float color = 3.0 - ...] line of code. else the edges will be insanely washed out white during the opening animation

	color *= smoothstep(0.5, 0.47, length(p)); // this creates an outside border to the portal which I can then clip (remove this line of code if you want to know exactly what it does, it will be very clear)

	float intensity = max(color, 0.0); // ensure we don't have negative values
	
	float4 noiseColor;
	if(gBluePortalColor)
		 noiseColor = float4( intensity*intensity*intensity*0.15, intensity*intensity*0.4, intensity , intensity);
	else
		 noiseColor = float4( intensity, intensity*intensity*0.4, intensity*intensity*intensity*0.15 , intensity);



	    // Interpolate the radius of the portal between 0 and gMaxClipRadius
    float portalRadius = lerp(0.0, gMaxClipRadius, portalScale);

	float radius = length(p);
	if(radius >= 0.47) // if radius is larger than this start clipping
	clip(intensity - 0.75);

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

// original:
// https://www.shadertoy.com/view/Wt3GRS#