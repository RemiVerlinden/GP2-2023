float4x4 gTransform : WorldViewProjection;
TextureCube  gSpriteTexture;
float2 gTextureSize = float2(1,1);

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(1,0,0,1); // Red color for debugging
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    DepthEnable = FALSE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
    uint TextureId : TEXCOORD0;
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 TexCoord : TEXCOORD0; // Make this a float3
};


//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
    return input;
}

//HELPER
//*************
float3 GetTexCoordAdjustment(float3 baseDir, float2 relPos)
{
    if(abs(baseDir.x) == 1) return float3(0, relPos.y, relPos.x);
    if(abs(baseDir.y) == 1) return float3(relPos.x, 0, relPos.y);
    if(abs(baseDir.z) == 1) return float3(relPos.x, relPos.y, 0);
    return float3(0, 0, 0);  // Fallback, shouldn't reach here
}


//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float3 baseDir, float2 relPos, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
		//Step 3.
		//Do rotation calculations
		//Transform to origin
		//Rotate
		//Retransform to initial position
		pos.x = pos.x - pivotOffset.x - offset.x;
		pos.y = pos.y - pivotOffset.y - offset.y;
		float x = pos.x;
		float y = pos.y;
		pos.x = x * rotCosSin.x - y * rotCosSin.y;
		pos.y = y * rotCosSin.x + x * rotCosSin.y;
		pos.x = pos.x + offset.x;
		pos.y = pos.y + offset.y;
    }
    else
    {
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		pos.x = pos.x - pivotOffset.x;
		pos.y = pos.y - pivotOffset.y;
    }

	//Geometry Vertex Output
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
   float3 adjustedDir = normalize(baseDir + GetTexCoordAdjustment(baseDir, relPos));
    geomData.TexCoord = adjustedDir;
    triStream.Append(geomData);
}

[maxvertexcount(36)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
    float2 size = gTextureSize;

    float3 position = float3(vertex[0].TransformData.x, vertex[0].TransformData.y, vertex[0].TransformData.z);		//Extract the position data from the VS_DATA vertex struct
    float2 offset = float2(vertex[0].TransformData.x, vertex[0].TransformData.y); 									//Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
    float rotation = vertex[0].TransformData.w; 									//Extract the rotation data from the VS_DATA vertex struct
    float2 pivot = float2(vertex[0].TransformData2.x, vertex[0].TransformData2.y); 	//Extract the pivot data from the VS_DATA vertex struct	
    float2 scale = float2(vertex[0].TransformData2.z, vertex[0].TransformData2.w); 	//Extract the scale data from the VS_DATA vertex struct

	float2 pivotOffset = float2(pivot.x * scale.x * gTextureSize.x, pivot.y * scale.y * gTextureSize.y);


    // Define the six look directions for the TextureCube faces
    float3 directions[6] = 
    {
    float3(1, 0, 0),   // Right
    float3(-1, 0, 0),  // Left
    float3(0, 1, 0),   // Top
    float3(0, -1, 0),  // Bottom
    float3(0, 0, 1),   // Front
    float3(0, 0, -1)   // Back
};


    // For each face
    for(int i = 0; i < 6; ++i)
    {
        float2 offset = float2((i % 3) * size.x, (i / 3) * size.y);

        // Bottom-left
        CreateVertex(triStream, float3(offset.x, offset.y + size.y, 0), vertex[0].Color, directions[i], float2(-1, -1), vertex[0].TransformData.z, float2(cos(vertex[0].TransformData.w), sin(vertex[0].TransformData.w)), offset, pivot);

        // Top-left
        CreateVertex(triStream, float3(offset.x, offset.y, 0), vertex[0].Color, directions[i], float2(-1, 1), vertex[0].TransformData.z, float2(cos(vertex[0].TransformData.w), sin(vertex[0].TransformData.w)), offset, pivot);

        // Bottom-right
        CreateVertex(triStream, float3(offset.x + size.x, offset.y + size.y, 0), vertex[0].Color, directions[i], float2(1, -1), vertex[0].TransformData.z, float2(cos(vertex[0].TransformData.w), sin(vertex[0].TransformData.w)), offset, pivot);

        // Top-left (repeat for winding order)
        CreateVertex(triStream, float3(offset.x, offset.y, 0), vertex[0].Color, directions[i], float2(-1, 1), vertex[0].TransformData.z, float2(cos(vertex[0].TransformData.w), sin(vertex[0].TransformData.w)), offset, pivot);

        // Bottom-right (repeat for winding order)
        CreateVertex(triStream, float3(offset.x + size.x, offset.y + size.y, 0), vertex[0].Color, directions[i], float2(1, -1), vertex[0].TransformData.z, float2(cos(vertex[0].TransformData.w), sin(vertex[0].TransformData.w)), offset, pivot);

        // Top-right
        CreateVertex(triStream, float3(offset.x + size.x, offset.y, 0), vertex[0].Color, directions[i], float2(1, 1), vertex[0].TransformData.z, float2(cos(vertex[0].TransformData.w), sin(vertex[0].TransformData.w)), offset, pivot);
    }
}



// //PIXEL SHADER
// //************
// float4 MainPS(GS_DATA input) : SV_TARGET
// {
//     float3 lookupDir = input.TexCoord;
//     float3 absDir = abs(lookupDir);
// if (absDir.x > absDir.y && absDir.x > absDir.z)
//     return (lookupDir.x > 0) ? float4(1, 0, 0, 1) : float4(0, 1, 0, 1);  // Red for +X, Green for -X
// else if (absDir.y > absDir.x && absDir.y > absDir.z)
//     return (lookupDir.y > 0) ? float4(0, 0, 1, 1) : float4(1, 1, 0, 1);  // Blue for +Y, Yellow for -Y
// else
//     return (lookupDir.z > 0) ? float4(0, 1, 1, 1) : float4(1, 0, 1, 1);  // Cyan for +Z, Magenta for -Z

// }

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    float4 sampledColor = gSpriteTexture.Sample(samPoint, input.TexCoord);
    return sampledColor * input.Color; // Multiply by vertex color if desired
}





// Default Technique
technique10 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		//SetDepthStencilState(NoDepth,0);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
