float4x4 gWorld;
float4x4 gLightViewProj;
float3 gLightPosition;
float gNearPlane;
float gFarPlane;
float4x4 gBones[125];
 float C = 20.f;

DepthStencilState depthStencilState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;

	DepthFunc = LESS_EQUAL;

};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = NONE;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader [STATIC]
//--------------------------------------------------------------------------------------
VS_OUTPUT ShadowMapVS(float3 position:POSITION)
{
	VS_OUTPUT output;

	//TODO: return the position of the vertex in correct space (hint: seen from the view of the light)
	float4 pos = float4(position, 1.0f);
	output.worldPos = mul(pos, gWorld);
	output.pos = mul(pos, mul(gWorld,gLightViewProj));
	
	return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader [SKINNED]
//--------------------------------------------------------------------------------------
VS_OUTPUT ShadowMapVS_Skinned(float3 position:POSITION, float4 BoneIndices : BLENDINDICES, float4 BoneWeights : BLENDWEIGHTS)
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
// Pixel Shaders
//--------------------------------------------------------------------------------------
void ShadowMapPS_VOID(float4 position:SV_POSITION){}

float ShadowMapPS_ESM(float4 position:SV_POSITION) : SV_DEPTH
{
    // Compute depth in [0,1] range
    float depth = position.z / position.w;
    // Convert depth into exponential form
    return exp(-C * depth);
}

float PS(VS_OUTPUT input) : SV_DEPTH{

	float depth = length(input.worldPos.xyz - gLightPosition);
	float normalizedDepth = (depth - gNearPlane) / (gFarPlane - gNearPlane);
	return normalizedDepth;
}

technique11 GenerateShadows
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
	    SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

technique11 GenerateShadows_Skinned
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS_Skinned()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}