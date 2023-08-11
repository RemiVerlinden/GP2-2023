float4x4 gWorld;
float4x4 gLightViewProjArray[6];
float4x4 gBones[125];
float C = 10.f;
DepthStencilState depthStencilState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = NONE;
};

struct VS_OUT
{
    float4 PositionWorld : SV_POSITION;
    uint Face : SV_RenderTargetArrayIndex;
};

struct GS_OUT
{
    float4 PositionWorld : SV_POSITION;
    uint RTArrayIndex : SV_RenderTargetArrayIndex;
};

//--------------------------------------------------------------------------------------
// Vertex Shader [STATIC]
//--------------------------------------------------------------------------------------
VS_OUT ShadowMapVS(float3 position:POSITION)
{
	VS_OUT output;

	// Transform vertex into world space
	// float4 pos = mul(float4(position, 1.0f), gWorld);
	// output.PositionWorld = pos;
	float4 pos = float4(position, 1.0f);
	output.PositionWorld = mul(pos, mul(gWorld, gLightViewProjArray[0]));

	output.Face = 0; // This is irrelevant for now, will be overridden in GS
	
	return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader [SKINNED]
//--------------------------------------------------------------------------------------
VS_OUT ShadowMapVS_Skinned(float3 position:POSITION, float4 BoneIndices : BLENDINDICES, float4 BoneWeights : BLENDWEIGHTS)
{
	//TODO: return the position of the ANIMATED vertex in correct space (hint: seen from the view of the light)
	float4 originalPosition = float4(position, 1.f);
	float4 transformedPosition = 0;
	for(int i = 0; i < 4; ++i)
	{
		float boneIndex = BoneIndices[i];
		if(boneIndex >= 0)
		{
			transformedPosition += 	BoneWeights[i] * mul(originalPosition, gBones[boneIndex]); 
		}
	}
	transformedPosition.w = 1;
	
	return ShadowMapVS(transformedPosition);
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(18)]
void GS(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> OutputStream)
{
    for (int face = 0; face < 6; face++)
    {
        for (int v = 0; v < 3; v++)
        {
            GS_OUT  outputVertex = input[v];

            // Transform the vertex's position by the view-projection matrix for the current face
            // outputVertex.PositionWorld = mul(outputVertex.PositionWorld, gLightViewProjArray[face]);
            outputVertex.RTArrayIndex  = face;

            OutputStream.Append(outputVertex);
        }
        OutputStream.RestartStrip();
    }
}

//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------
void ShadowMapPS_VOID(float4 position:SV_POSITION){}

// this is for testing purpose, it makes the depthbuffer more easy to see during sprite render
float ShadowMapPS_ESM(float4 position:SV_POSITION) : SV_DEPTH
{
    // Compute depth in [0,1] range
    float depth = position.z / position.w;
    // Convert depth into exponential form
    return exp(-C * depth);
}

float ShadowMapPSNULL(float4 position:SV_POSITION) : SV_DEPTH
{
    return 0.0;
}


float ShadowMapPS(float4 position:SV_POSITION, uint RTArrayIndex : SV_RenderTargetArrayIndex) : SV_Depth
{
    switch(RTArrayIndex)
    {
        case 0: return 0.2;
        case 1: return 0.4;
        case 2: return 0.6;
        case 3: return 0.8;
        case 4: return 1.0;
        case 5: return 0.0; // This will be closest (black)
        default: return 1.0; // Fallback
    }
}



technique11 GenerateShadows
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader(CompileShader(gs_4_0, GS()));
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPSNULL()));
	}
}

technique11 GenerateShadows_Skinned
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS_Skinned()));
		SetGeometryShader(CompileShader(gs_4_0, GS()));
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPSNULL()));
	}
}