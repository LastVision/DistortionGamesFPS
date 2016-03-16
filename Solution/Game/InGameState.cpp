#include "stdafx.h"

#include "ClientLevel.h"
#include "ClientLevelFactory.h"
#include "Console.h"
#include <GameStateMessage.h>
#include <EffectContainer.h>
#include <FadeMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "ServerSelectState.h"
#include <NetMessageRequestLevel.h>
#include <PostMaster.h>
#include <ScriptSystem.h>
#include <VTuneApi.h>

#include <Cursor.h>
#include "ClientNetworkManager.h"

InGameState::InGameState(int aLevelID)
	: myGUIManager(nullptr)
	, myLevelToLoad(aLevelID)
	, myShouldLoadLevel(true)
	, myLevel(nullptr)
{
	myIsActiveState = false;
	myLevelFactory = new ClientLevelFactory("Data/Level/LI_level.xml");
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
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
	myCursor->SetShouldRender(false);

	//PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));
	//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(myLevelToLoad));

	myIsActiveState = true;

	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
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
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_N))
	{
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new ServerSelectState());
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_NUMPAD1))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(0));
		//SET_RUNTIME(false);
		//SAFE_DELETE(myLevel);
		//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(0));
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_NUMPAD2))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(1));
		//SET_RUNTIME(false);
		//SAFE_DELETE(myLevel);
		//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(1));
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_NUMPAD3))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(2));
		//SET_RUNTIME(false);
		//SAFE_DELETE(myLevel);
		//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(2));
	}

	if (myShouldLoadLevel == true)
	{
		myShouldLoadLevel = false;
		SET_RUNTIME(false);
		SAFE_DELETE(myLevel);
		myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(myLevelToLoad));
		myLevelToLoad = -1;
	}

	DL_ASSERT_EXP(myLevel != nullptr, "Invalid level");
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
	myLevelToLoad = -1;
}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.myGameState)
	{
	case eGameState::LOAD_LEVEL:
		myLevelToLoad = aMessage.myID;
		break;
	}
}

void InGameState::OnResize(int, int)
{
}