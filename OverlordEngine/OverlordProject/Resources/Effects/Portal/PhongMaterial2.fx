

float4x4 gMatrixWVP : WORLDVIEWPROJECTION;
float4x4 gMatrixViewInverse : VIEWINVERSE;
float4x4 gMatrixWorld : WORLD;

//STATES
//******
RasterizerState gRS_FrontCulling 
{ 
	CullMode = FRONT; 
};

BlendState gBS_EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 8; // Adjust the value based on your requirements
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

//LIGHT
//*****
float3 gLightDirection : DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool gUseHalfLambert = true;

bool gUseTextureDiffuse = false;

float4 gColorDiffuse = float4(1,1,1,1);

Texture2D gTextureDiffuse
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
>;

//SPECULAR
//********
float4 gColorSpecular = float4(1,1,1,1);

Texture2D gTextureSpecularIntensity
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Texture";
>;

bool gUseTextureSpecularIntensity
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Bool";
> = false;

int gShininess = 15;

//AMBIENT
//*******
float4 gColorAmbient = float4(0,0,0,1);

float gAmbientIntensity = 0.0f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel = false;

bool gUseTextureNormal = false;

Texture2D gTextureNormal
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
>;

//ENVIRONMENT MAPPING
//*******************
TextureCube gCubeEnvironment
<
	string UIName = "Environment Cube";
	string ResourceType = "Cube";
>;

bool gUseEnvironmentMapping = false;

float gReflectionStrength = 0.0f;

float gRefractionStrength = 0.0f;

float gRefractionIndex = 0.3f;

//FRESNEL FALLOFF
//***************
bool gUseFresnelFalloff = false;


float4 gColorFresnel = float4(1,1,1,1);

float gFresnelPower = 2.0f;

float gFresnelMultiplier = 1.5;

float gFresnelHardness = 2.0;

//OPACITY
//***************
float gOpacityIntensity = 1.0f;

bool gUseOpacityMap = false;

Texture2D gTextureOpacity
<
	string UIName = "Opacity Map";
	string UIWidget = "Texture";
>;


//SPECULAR MODELS
//***************
bool gUseSpecularPhong = false;

//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: COLOR0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

float3 CalculateSpecularPhong(float3 viewDirection, float3 normal, float2 texCoord)
{
	const float3 reflectedVector = -reflect(gLightDirection,normal);

	float RdotV = saturate(dot(reflectedVector,viewDirection));
	float specPower = pow(RdotV, gShininess);
	
	float3 specularColor  = gColorSpecular.rgb * specPower;
	
	return specularColor;
}



float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{	
	float3 specColor = CalculateSpecularPhong(viewDirection,normal,texCoord);

	if (gUseTextureSpecularIntensity)
	{
		specColor *= gTextureSpecularIntensity.Sample(gTextureSampler, texCoord);
	}

	return specColor;
}

float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
	if (gUseTextureNormal)
	{
		float3 binormal = normalize(cross(normal, tangent));
		const float3x3 localAxis = float3x3(tangent,binormal,normal);

		//extract normal from normal map and transform to [-1,1] range
		float3 sampledNormal = gTextureNormal.Sample(gTextureSampler, texCoord).rgb;
		sampledNormal = sampledNormal*2-1;
		
		// Flip the green channel of the sampled normal if necessary
		if (gFlipGreenChannel)
		{
			sampledNormal.g = -sampledNormal.g;

		}

		normal = normalize(mul(sampledNormal, localAxis));
	}

	return normal;
}

float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	float3 diffuseColor = gColorDiffuse;
	if (gUseTextureDiffuse)
	{
		diffuseColor *= gTextureDiffuse.Sample(gTextureSampler, texCoord);
	}
	float lambertFactor = dot(-normal, gLightDirection);
	if (gUseHalfLambert)
	{
		lambertFactor = ((lambertFactor*0.5f)+0.5f);
		lambertFactor *= lambertFactor; 
	}
	// lambertFactor = saturate(lambertFactor);

	diffuseColor *= lambertFactor;
	return diffuseColor;
}

float3 CalculateFresnelFalloff(float3 normal, float3 viewDirection, float3 environmentColor)
{
	if (gUseFresnelFalloff == false)
	{
		return gUseEnvironmentMapping ? environmentColor : float3(0,0,0);
	}
	const float fresnel = 1-pow(saturate(abs(dot(normal,viewDirection))),gFresnelPower)*gFresnelMultiplier;
	const float fresnelMask = pow(1-saturate(dot(float3(0,-1,0),normal)),gFresnelHardness);
	const float3 color = gUseEnvironmentMapping ? environmentColor : gColorFresnel;
	return fresnel * fresnelMask * color;
}

float3 CalculateEnvironment(float3 viewDirection, float3 normal)
{
	float3 environmentColor = float3(0,0,0);
	
	if (gUseEnvironmentMapping == false)
	{
		return environmentColor;
	}
	
	const float3 reflectedView = normalize(reflect(viewDirection, normal));
	const float3 refractedView = normalize(refract(viewDirection, normal, gRefractionIndex));
	
	environmentColor = gCubeEnvironment.Sample(gTextureSampler, reflectedView) * gReflectionStrength;
	environmentColor += gCubeEnvironment.Sample(gTextureSampler, refractedView) * gRefractionStrength;
	
	return environmentColor;
}

float CalculateOpacity(float2 texCoord)
{
	float opacity = gOpacityIntensity;
	
	if (gUseOpacityMap)
	{
		opacity *= gTextureOpacity.Sample(gTextureSampler,texCoord).r;
	}
	
	return opacity;
}

// The main vertex shader
VS_Output MainVS(VS_Input input) {
	
	VS_Output output = (VS_Output)0;
	
	output.Position = mul(float4(input.Position, 1.0), gMatrixWVP);
	output.WorldPosition = mul(float4(input.Position,1.0), gMatrixWorld);
	output.Normal = mul(input.Normal, (float3x3)gMatrixWorld);
	output.Tangent = mul(input.Tangent, (float3x3)gMatrixWorld);
	output.TexCoord = input.TexCoord;
	
	return output;
}

// The main pixel shader
float4 MainPS(VS_Output input) : SV_TARGET {
	// NORMALIZE
	float3 normal = normalize(input.Normal);
	float3 tangent = normalize(input.Tangent);
	
	float3 viewDirection = normalize(input.WorldPosition.xyz - gMatrixViewInverse[3].xyz);
	
	//NORMAL
	float3 newNormal = CalculateNormal(tangent, normal, input.TexCoord);
			
	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, newNormal, input.TexCoord);
		
	//DIFFUSE
	float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);
		
	//AMBIENT
	float3 ambientColor = gColorAmbient * gAmbientIntensity;
		
	//ENVIRONMENT MAPPING
	float3 environmentColor = CalculateEnvironment(viewDirection, newNormal);
	
	//FRESNEL FALLOFF
	environmentColor = CalculateFresnelFalloff(newNormal, viewDirection, environmentColor);
		
	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor + environmentColor + ambientColor;
	
	//OPACITY
	float opacity = CalculateOpacity(input.TexCoord);
	
	return float4(finalColor,opacity);
}

// Default Technique
technique10 WithAlphaBlending {
	pass p0 {
		SetRasterizerState(gRS_FrontCulling);
		SetBlendState(gBS_EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

// Default Technique
technique10 WithoutAlphaBlending {
	pass p0 {
		SetRasterizerState(gRS_FrontCulling);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}