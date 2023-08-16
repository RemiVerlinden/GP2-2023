//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
Texture2D gDepthTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};
/// Create Rasterizer State (Backface culling) 

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	// Set the Position
    output.Position = float4(input.Position,1);
	// Set the TexCoord
	output.TexCoord = input.TexCoord;
	return output;
}



// ---------------------------
// pseudo random number generator
// ---------------------------
int seed = 1;
void srand(int s) 
{ 
    seed = s; 
}

int rand() 
{ 
    seed = seed * 0x343fd + 0x269ec3; 
    return (seed >> 16) & 32767; 
}

float frand() 
{ 
    return float(rand()) / 32767.0; 
}

// ---------------------------
// hash function
// ---------------------------
int hash(int n) 
{ 
    n = (n << 13) ^ n; 
    return n * (n * n * 15731 + 789221) + 1376312589; 
}

// ---------------------------
// helper functions
// ---------------------------
float3 sphereVolumeRandPoint()
{
    float3 p = float3(frand(), frand(), frand()) * 2.0 - 1.0;
    while(length(p) > 1.0)
    {
        p = float3(frand(), frand(), frand()) * 2.0 - 1.0;
    }
    return p;
}

float depth(float2 coord)
{
    float2 uv = coord * float2(iResolution[1] / (float)iResolution[0], 1.0);
    return gDepthTexture.Sample(gDepthTextureSampler, uv).x;
}

float SSAO(float2 coord)
{
    float cd = depth(coord);
    float screenRadius = 0.5 * (AOradius / cd) / 0.53135;
    float li = 0.0;
    float count = 0.0;
    for(int i = 0; i < Samples; i++)
    {
        float3 p = sphereVolumeRandPoint() * frand();
        float2 sp = float2(coord.x + p.x * screenRadius, coord.y + p.y * screenRadius);
        float d = depth(sp);
        float at = pow(length(p) - 1.0, 2.0);
        li += step(cd + p.z * AOradius, d) * at;
        count += at;
    }
    return li / count;
}

float3 background(float yCoord) 
{	    
    return lerp(float3(0.1515, 0.2375, 0.5757), float3(0.0546, 0.0898, 0.1953), yCoord);
}

// ---------------------------
// pixel shader
// ---------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    // init random seed
    int2 q = int2(input.TexCoord);
    srand(hash(q.x + hash(q.y + hash(1117 * iFrame))));
    
    // coordinate
    float2 uv = input.TexCoord / float2(iResolution[0], iResolution[1]);
    float2 coord = input.TexCoord / iResolution[1];
    
    float d = depth(coord);
    float3 ao = float3(0.4) + step(d, 1e5 - 1.0) * float3(0.8) * SSAO(coord);
    float3 color = lerp(background(uv.y), ao, 1.0 - smoothstep(0.0, 0.99, d * d / 1e3));
    
    color = pow(color, float3(1.0 / 2.2)); // gamma
    return float4(color, 1.0);
}



//TECHNIQUE
//---------
technique11 PostProcess
{
    pass P0
    {          
		SetDepthStencilState(EnableDepth, 0);
        // Set states...
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

