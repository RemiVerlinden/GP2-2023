#include "stdafx.h"
#include "TriggerTestScene.h"
#include "PhysxManager.h"
#include "SpherePosColorNorm.h"
#include "Logger.h"

void TriggerTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysX->createMaterial(1.f, 1.f, .2f);

	// Ground plane
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ {PxPiDivTwo,{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// Sphere
	m_pSphere = new SpherePosColorNorm(1, 10, 10, XMFLOAT4{Colors::Gray});
	AddGameObject(m_pSphere);

	auto pSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
	PxRigidActorExt::createExclusiveShape(*pSphereActor, PxSphereGeometry{ 1.f }, *pDefaultMaterial);

	m_pSphere->AttachRigidActor(pSphereActor);
	m_pSphere->Translate(0, 2, 0);

	// left trigger
	m_pTriggerLeft = pPhysX->createRigidStatic(PxTransform{ -8,0.5f,0 });
	auto pShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerLeft, PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *pDefaultMaterial);

	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	m_pPhysxScene->addActor(*m_pTriggerLeft);

	// right trigger
	m_pTriggerRight = pPhysX->createRigidStatic(PxTransform{ 8,0.5f,0 });
	pShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerRight, PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *pDefaultMaterial);

	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	m_pPhysxScene->addActor(*m_pTriggerRight);
}

void TriggerTestScene::Update()
{
	float force = 5.f;
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_LEFT))
	{
		// only rigidDynamic can have torque applied to we have to tell it specifically that we are
		// dealing with a rigid DYNAMIC actor
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addTorque({ 0,0,force });
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_RIGHT))
	{
		// only rigidDynamic can have torque applied to we have to tell it specifically that we are
		// dealing with a rigid DYNAMIC actor
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addTorque({ 0,0,-force });
	}

	if (m_IsLeftTriggered)
	{
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addForce({ 0,10,0 }, PxForceMode::eIMPULSE);
		m_IsLeftTriggered = false;
	}
}

void TriggerTestScene::Draw() const
{
}

void TriggerTestScene::OnSceneActivated()
{
}

void TriggerTestScene::OnSceneDeactivated()
{
}

void TriggerTestScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i)
	{
		// ignore if deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_OTHER | PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER))
			continue;

		const PxTriggerPair pair = pairs[i];
		if (pair.triggerActor == m_pTriggerLeft)
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTER
			{
				Logger::GetInstance()->LogInfo(L"ENTERED trigger LEFT");

				PxVec3 force = { 1,0,0 };
				m_IsLeftTriggered = true;
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST) // LEFT
			{
				Logger::GetInstance()->LogInfo(L"LEFT trigger LEFT");

			}
		}
		if (pair.triggerActor == m_pTriggerRight)
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTER
			{
				Logger::GetInstance()->LogInfo(L"ENTERED trigger RIGHT");
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST) // LEFT
			{
				Logger::GetInstance()->LogInfo(L"LEFT trigger RIGHT");

			}
		}
	}
}
