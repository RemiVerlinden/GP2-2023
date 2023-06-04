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

Texture3D gColorCorrectionLUT;

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


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
    // Step 1: sample the texture
    float4 sampleColor = gTexture.Sample(samPoint, input.TexCoord);

    // Step 2: Apply color correction
    float4 correctedColor = gColorCorrectionLUT.Sample(samPoint, sampleColor.rgb);
    correctedColor.a = sampleColor.a; // preserve alpha

    return correctedColor;
}


//TECHNIQUE
//---------
technique11 PostProcess
{
    pass P0
    {          
        // Set states...
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

