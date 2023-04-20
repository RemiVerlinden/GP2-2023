//INCOMPLETE!

float4x4 gTransform : WORLDVIEWPROJECTION;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel : TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{
	//Create a new GS_DATA object
	GS_DATA output;
	//Fill in all the fields
	output.Position =mul(float4(pos,1.0f), gTransform);
	output.Color = col;
	output.TexCoord = texCoord;
	output.Channel = channel;
	//Append it to the TriangleStream
	triStream.Append(output);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Create a Quad using the character information of the given vertex
	//Note that the Vertex.CharSize is in screenspace, TextureCoordinates aren't ;) [Range 0 > 1]

	float3 cornerPos;
	float2 cornerTexCoord;
	float2 charSizeTexCoord; 
	charSizeTexCoord.x = vertex[0].CharSize.x / gTextureSize.x;
	charSizeTexCoord.y = vertex[0].CharSize.y / gTextureSize.y;
	
	//1. Vertex Left-Top
	cornerPos = vertex[0].Position;
	cornerTexCoord= vertex[0].TexCoord;
	CreateVertex(triStream, cornerPos, vertex[0].Color, cornerTexCoord, vertex[0].Channel);

	// 2. Vertex Right-Top
	cornerPos.x += vertex[0].CharSize.x;
	cornerTexCoord.x += charSizeTexCoord.x;
	CreateVertex(triStream, cornerPos, vertex[0].Color, cornerTexCoord, vertex[0].Channel);

	// 3. Vertex Left-Bottom

	cornerPos += float3(-vertex[0].CharSize.x,vertex[0].CharSize.y,0);
	cornerTexCoord += float2(-charSizeTexCoord.x, charSizeTexCoord.y);
	CreateVertex(triStream, cornerPos, vertex[0].Color, cornerTexCoord, vertex[0].Channel);

	// 4. Vertex Right-Bottom
	cornerPos.x += vertex[0].CharSize.x;
	cornerTexCoord.x += charSizeTexCoord.x;
	CreateVertex(triStream, cornerPos, vertex[0].Color, cornerTexCoord, vertex[0].Channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET{

	//Sample the texture and return the correct channel [Vertex.Channel]
	//You can iterate a float4 just like an array, using the index operator
	//Also, don't forget to colorize ;) [Vertex.Color]
	float4 sampledColor = gSpriteTexture.Sample(samPoint,input.TexCoord);
	float channelColor = sampledColor[input.Channel];

	float4 color = channelColor > 0.0 ? channelColor * input.Color : float4(0, 0, 0, 0);
	
	return color;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
