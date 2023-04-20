#include "stdafx.h"
#include "SceneSelector.h"
#include "SceneManager.h"

//Change this define to activate/deactive the corresponding scenes
// W01 - W02  (#define ...)

//#define W01
//#define W02
#define MILESTONE

#ifdef W01
#include "TestScene.h"
#include "PhysXTestScene.h"
#include "BouncingBallScene.h"
#include "FrictionScene.h"
#include "ForcesScene.h"
#include "TorqueScene.h"
#include "W1_AssignmentScene.h"
#endif

#ifdef W02
#include "PhysXMeshTestScene.h"
#include "TriggerTestScene.h"
#include "AudioTestScene.h"
#include "W2_AssignmentScene.h"
#endif

#ifdef MILESTONE
#include "W1_AssignmentScene.h"
#include "W2_AssignmentScene.h"
#endif

namespace dae
{
	void AddScenes(SceneManager * pSceneManager)
	{

#ifdef W01
		pSceneManager->AddGameScene(new W1_AssignmentScene());
		pSceneManager->AddGameScene(new ForcesScene());
		pSceneManager->AddGameScene(new TorqueScene());
		pSceneManager->AddGameScene(new FrictionScene());
		pSceneManager->AddGameScene(new PhysXTestScene());
		pSceneManager->AddGameScene(new BouncingBallScene());
		pSceneManager->AddGameScene(new TestScene());
#endif

#ifdef W02
		pSceneManager->AddGameScene(new W2_AssignmentScene());
		pSceneManager->AddGameScene(new AudioTestScene());
		pSceneManager->AddGameScene(new TriggerTestScene());
		pSceneManager->AddGameScene(new PhysXMeshTestScene());
#endif

#ifdef MILESTONE
		pSceneManager->AddGameScene(new W1_AssignmentScene());
		pSceneManager->AddGameScene(new W2_AssignmentScene());

#endif

	}
}

