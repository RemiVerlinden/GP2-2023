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

float aberrationAmount = 0.02f;  // adjust this value as required
float2 screenCenter = float2(0.5, 0.5);  // Assuming the center of the screen is at (0.5, 0.5)

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
// Calculate the direction and distance from the screen center to the current pixel
    float2 direction = input.TexCoord - screenCenter;
    float distanceFromCenter = length(direction);

    // Intensify the direction magnitude using the distance from the center
    direction = normalize(direction) * aberrationAmount * distanceFromCenter;

    // Sample the texture for each color channel with the adjusted UVs
    float4 redChannel = gTexture.Sample(samPoint, input.TexCoord + direction);
    float4 greenChannel = gTexture.Sample(samPoint, input.TexCoord);
    float4 blueChannel = gTexture.Sample(samPoint, input.TexCoord - direction);

    // Construct the final color using channels from the samples
    return float4(redChannel.r, greenChannel.g, blueChannel.b, 1.0f); // Assuming you want the alpha to be fully opaque
}


//TECHNIQUE
//---------
technique11 CAA
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}