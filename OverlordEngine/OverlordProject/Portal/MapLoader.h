#pragma once
class MapLoader final
{
	enum class ShaderType
	{
		noDraw,
		decal,
		diffuse,
		light,
		PhongProp, // phong props have a "normal" & "exponent" map
		PhongBrush  // phong brushes only have an "exponent" map
	};

	struct SubmeshShaderInfo
	{
		ModelComponent* pModel;
		ShaderType shaderType;
		UINT8 submeshID;

		const fs::path* pMapPath; // this is just so I dont have to copy the map path to this each time we loop through a submesh
		std::wstring materialName; // same thing as above but for the material name

		fs::path diffusePath;
		fs::path normalPath;
		fs::path specularPath;
	};

public:
	MapLoader(GameScene& gamescene);

	[[maybe_unused]]GameObject* LoadMap(const std::wstring& mapName);

private:
	void LoadMapTexturesDebug(const std::wstring& mapName, ModelComponent* pMapModel);
	void LoadMapTexturesRelease(const std::wstring& mapName, ModelComponent* pMapModel);
	
	void FindTexurePaths(SubmeshShaderInfo& shaderInfo);

	void LoadNoDrawShaderForSubmesh(SubmeshShaderInfo& shaderInfo);
	void LoadDecalShaderForSubmesh(SubmeshShaderInfo& shaderInfo);
	void LoadDiffuseShaderForSubmesh(SubmeshShaderInfo& shaderInfo);
	void LoadPhongShaderForSubmesh(SubmeshShaderInfo& shaderInfo);
	void LoadLightShaderForSubmesh(SubmeshShaderInfo& shaderInfo);

	fs::path ShortenTexturePath(const fs::path& texturePath);

	ShaderType IdentifyShaderType(const std::wstring& submeshName, SubmeshShaderInfo& shaderInfo);

	GameScene& m_Scene;
};
