#include "stdafx.h"

#include "Console.h"
#include <EffectContainer.h>
#include <FadeMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "Level.h"
#include "LevelFactory.h"
#include <MemoryTracker.h>
#include <PostMaster.h>
#include <ScriptSystem.h>
#include <VTuneApi.h>

InGameState::InGameState()
	: myGUIManager(nullptr)
{
	myIsActiveState = false;
	myLevelFactory = new LevelFactory("Data/Level/LI_level.xml");
}

InGameState::~InGameState()
{
	Console::Destroy();
	SAFE_DELETE(myLevel);
	SAFE_DELETE(myLevelFactory);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;

	//PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));
	myLevel = myLevelFactory->LoadCurrentLevel();

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

	myLevel->Update(aDeltaTime);

	//LUA::ScriptSystem::GetInstance()->CallFunction("Update", { aDeltaTime });
	//LUA::ScriptSystem::GetInstance()->Update();

	
	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	myLevel->Render();

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