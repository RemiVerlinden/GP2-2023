#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	TODO_W9(L"Implement Destructor")
		delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Initialize")
	if(!m_pParticleMaterial) m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(L"Labs/Week9/Textures/Smoke.png");

}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement CreateVertexBuffer")

	if(m_pVertexBuffer) m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // Set the usage to DYNAMIC
	bufferDesc.ByteWidth = sizeof(VertexParticle) * m_MaxParticles; // Replace VertexType with your vertex data type, and numVertices with the number of vertices
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Use VERTEX_BUFFER or INDEX_BUFFER, depending on your needs
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Allow CPU to write to the buffer
	bufferDesc.MiscFlags = 0;
	
	HRESULT hr = sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pVertexBuffer);

	if (FAILED(hr))
	{
		Logger::LogError(L"BIG ERROR BLA BLA GO TO THIS PLACE LOOK BAD");
		assert(false);
	}
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Update")
	const float timeStep = sceneContext.pGameTime->GetElapsed();

	float particleInterval = (m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / m_ParticleCount;
	m_LastParticleSpawn += timeStep;
	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE pData;
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);

	VertexParticle* pBuffer = reinterpret_cast<VertexParticle*>(pData.pData);

	for (UINT particle = 0; particle < m_ParticleCount; ++particle)
	{
		Particle& p = m_ParticlesArray[particle];
		if (p.isActive)
		{
			UpdateParticle(p, timeStep);
		}
		if (!p.isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(p);
			m_LastParticleSpawn -= particleInterval;
		}
		if (p.isActive) 
		{
			pBuffer[m_ActiveParticles] = p.vertexInfo;
			++m_ActiveParticles;
		}
	}
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	TODO_W9(L"Implement UpdateParticle")
		p.currentEnergy -= elapsedTime;
	if (p.currentEnergy <= 0.0f)
	{
		p.isActive = false;
		return;
	}
	VertexParticle& info = p.vertexInfo;

	// Convert XMFLOAT3 to XMVECTOR
	XMVECTOR lhsVec = XMLoadFloat3(&info.Position);
	XMVECTOR rhsVec = XMLoadFloat3(&m_EmitterSettings.velocity);

	// Perform XMVECTOR = XMVECTOR + XMVECTOR * FLOAT
	XMVECTOR resultVec = XMVectorAdd(lhsVec, XMVectorScale(rhsVec, elapsedTime));

	// Store the result back in XMFLOAT3
	XMStoreFloat3(&info.Position, resultVec);
	
	float lifePercentage = p.currentEnergy / p.totalEnergy;
	info.Color.w = m_EmitterSettings.color.w * lifePercentage;

	p.vertexInfo.Size = (1.0f - lifePercentage) * (p.initialSize * p.sizeChange) + lifePercentage * p.initialSize;
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;


	XMVECTOR randomDir{ 1,0,0 };

	randomDir = XMVector3TransformNormal(randomDir, XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI)));

	float randomDistance{ MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };

	//d.Everything is in place to calculate the initial position
		//i.vertexInfo.Position = ‘our random direction’ * ‘our random distance’
	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&GetTransform()->GetWorldPosition()) + randomDir * randomDistance);

	//4. Size Initialization 
	//a.Our vertexInfo.Size and initialSize are both equal to a random value that lays between MinSize and MaxSize(see EmitterSettings)
	p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.vertexInfo.Size = p.initialSize;

	//b.sizeChange is equal to a random value that lays between minScaleand maxScale(see EmitterSettings)
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	//5. Rotation Initialization 
	//a.The rotation(vertexInfo.Rotation) is a random value between –PIand PI.
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	//6. Color Initialisation
	//a.The particle’s color(vertexInfo.Color) is equal to the color from the emitter settings.
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement PostDraw");

	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());

	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());


	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);


	MaterialTechniqueContext tech = m_pParticleMaterial->GetTechniqueContext();

	//3. Set the InputLayout
	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(tech.pInputLayout);


	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride{ sizeof(VertexParticle) };
	UINT offset{ 0 };
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech.pTechnique->GetDesc(&techDesc);
	for (UINT i{ 0 }, passes{ techDesc.Passes }; i < passes; ++i)
	{

		tech.pTechnique->GetPassByIndex(i)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}