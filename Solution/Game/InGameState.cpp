#include "stdafx.h"

#include <Camera.h>
#include "Console.h"
#include <EffectContainer.h>
#include <FadeMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <Instance.h>
#include <MemoryTracker.h>
#include <ModelLoader.h>
#include <PlayerComponent.h>
#include <PostMaster.h>
#include <Scene.h>
#include <ScriptSystem.h>
#include <VTuneApi.h>

InGameState::InGameState()
	: myGUIManager(nullptr)
{
	myIsActiveState = false;
}

InGameState::~InGameState()
{
	Console::Destroy();
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;

	//PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));

	myPlayer = new PlayerComponent();
	myScene = new Prism::Scene(*myPlayer->myCamera);
	myInstance = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_muzzleflash.fbx", "Data/Resource/Shader/S_effect_pbl.fx")
		, myInstanceOrientation, Prism::eOctreeType::DYNAMIC, myCullingRadius);
	myScene->AddInstance(myInstance);

	myIsActiveState = true;
}

void InGameState::EndState()
{
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	Prism::EffectContainer::GetInstance()->Update(aDeltaTime);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		return eStateStatus::ePopMainState;
	}

	myPlayer->Update(aDeltaTime);

	//LUA::ScriptSystem::GetInstance()->CallFunction("Update", { aDeltaTime });
	//LUA::ScriptSystem::GetInstance()->Update();

	
	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	myScene->Render(false);

	VTUNE_EVENT_END();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void InGameState::OnResize(int aWidth, int aHeight)
{
}