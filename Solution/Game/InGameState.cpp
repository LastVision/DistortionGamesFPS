#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include "Console.h"
#include <ColoursForBG.h>
#include "ConsoleState.h"
#include <Cursor.h>
#include <EffectContainer.h>
#include <Engine.h>
#include <Entity.h>
#include <GameStateMessage.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <MemoryTracker.h>
#include <ModelLoader.h>
#include <MoveCameraMessage.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <TimerManager.h>
#include <TriggerMessage.h>
#include <VTuneApi.h>
#include <Vector.h>

#include <ScriptSystem.h>
#include <CinematicMessage.h>
#include <RunScriptMessage.h>
#include <FadeMessage.h>

InGameState::InGameState()
	: myGUIManager(nullptr)
{
	myIsActiveState = false;
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::MOVE_CAMERA, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TRIGGER, this);

	Console::Destroy();
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;

	PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));

	myIsActiveState = true;
}

void InGameState::EndState()
{
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	Prism::EffectContainer::GetInstance()->Update(aDeltaTime);

	LUA::ScriptSystem::GetInstance()->CallFunction("Update", { aDeltaTime });
	LUA::ScriptSystem::GetInstance()->Update();

	
	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	
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