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


// Constants
static const int   samplesX    = 15;  // must be odd
static const int   samplesY    = 15;  // must be odd
static const int   halfSamplesX = samplesX / 2;
static const int   halfSamplesY = samplesY / 2;

float4 BlurredPixel (float2 uv, float2 pixelSize)
{   
    float3 ret = float3(0, 0, 0);        
    for (int y = -halfSamplesY; y <= halfSamplesY; ++y)
    {
        for (int x = -halfSamplesX; x <= halfSamplesX; ++x)
        {
            float2 offset = float2(x, y) * pixelSize;
            ret += gTexture.Sample(samPoint, uv + offset).rgb;
        }
    }
    ret /= (samplesX * samplesY);
    return float4(ret, 1.0);
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Retrieve the texture size
	uint2 textureSize;
    gTexture.GetDimensions(textureSize.x, textureSize.y);
	
	// Calculate pixel size
	float2 pixelSize = 1.0 / float2(textureSize.x, textureSize.y);

	// Calculate the UV coordinates and pass to the BlurredPixel function
	return BlurredPixel(input.TexCoord, pixelSize);
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}