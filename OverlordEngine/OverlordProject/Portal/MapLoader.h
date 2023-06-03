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

	void LoadDynamicProps(const std::wstring& mapName);

	enum class DynamicPropType
	{
		button,
		elevator,
		door,
		cube
	};

	struct CubeProperties
	{
		std::array<float, 3> pxMaterial{0.2f, 0.2f, 0.27f};
		std::wstring diffuseMapPath = L"Textures/Maps/chamber02_static/materials_models_props_metal_box.dds";
		std::wstring normalMapPath = L"Textures/Maps/chamber02_static/materials_models_props_metal_box_normal.dds";

		std::wstring modelPath = L"Meshes/DynamicProps/cube.ovm";
		std::wstring rigidBodyPath = L"Meshes/DynamicProps/cube.ovpm";
		float mass = 10.f;
	};

	struct ButtonProperties
	{
		std::array<float, 3> pxMaterial{0.7f, 0.7f, 0.0f};
		std::wstring diffuseMapPath = L"Textures/Maps/chamber02_static/materials_models_props_button.dds";
		std::wstring normalMapPath = L"Textures/Maps/chamber02_static/materials_models_props_button_normal.dds";

		std::wstring modelPath = L"Meshes/DynamicProps/button_top.ovm";
		std::wstring rigidBodyPath = L"Meshes/DynamicProps/button_top.ovpm";
		float mass = 10.f;
	};

	struct DoorProperties
	{
		std::array<float, 3> pxMaterial{0.7f, 0.7f, 0.0f};
		std::wstring diffuseMapPath = L"Textures/Maps/chamber02_static/materials_models_props_door_01.dds";
		std::wstring normalMapPath = L"Textures/Maps/chamber02_static/materials_models_props_door_01_normal.dds";

		std::wstring modelPath = L"Meshes/DynamicProps/door.ovm";
		std::wstring rigidBodyPath = L"Meshes/DynamicProps/door.ovpc";
	};

	void SpawnButton(const XMFLOAT3& position);
	void SpawnElevator(const XMFLOAT3& position);
	void SpawnDoor(const XMFLOAT3& position);
	void SpawnCube(const XMFLOAT3& position);

	GameScene& m_Scene;

	struct InteractiveElements 
	{
		std::vector<GameObject*> buttons;
		std::vector<GameObject*> elevators;
		std::vector<GameObject*> doors;
	};
	InteractiveElements m_InteractiveElements;
};
