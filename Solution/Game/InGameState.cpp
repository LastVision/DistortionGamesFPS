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
#include <NetMessageAllClientsComplete.h>
#include <NetMessageLevelComplete.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageLoadLevel.h>
#include <NetMessageRequestLevel.h>
#include <NetMessageRequestStartLevel.h>
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
	, myLevelComplete(false)
	, myCanStartNextLevel(false)
{
	myIsActiveState = false;
	myLevelFactory = new ClientLevelFactory("Data/Level/LI_level.xml");
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ALL_CLIENTS_COMPLETE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LOAD_LEVEL, this);
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
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ALL_CLIENTS_COMPLETE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::LOAD_LEVEL, this);
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

	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_NUMPAD1))
	//{
	//	ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(0));
	//	//SET_RUNTIME(false);
	//	//SAFE_DELETE(myLevel);
	//	//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(0));
	//}
	//else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_NUMPAD2))
	//{
	//	ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(1));
	//	//SET_RUNTIME(false);
	//	//SAFE_DELETE(myLevel);
	//	//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(1));
	//}
	//else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_NUMPAD3))
	//{
	//	ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(2));
	//	//SET_RUNTIME(false);
	//	//SAFE_DELETE(myLevel);
	//	//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(2));
	//}



	if (myShouldLoadLevel == true)
	{
		myShouldLoadLevel = false;
		SET_RUNTIME(false);
		SAFE_DELETE(myLevel);
		myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(myLevelToLoad));
		myLevelToLoad = -1;
	}

	if (myLevelComplete == true)
	{
		DEBUG_PRINT("LEVEL COMPLETE");

		if (myCanStartNextLevel == true)
		{
			DEBUG_PRINT("Press space to continue!");

			if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true)
			{
				ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
			}
		}
	}
	else
	{
		DL_ASSERT_EXP(myLevel != nullptr, "Invalid level");
		myLevel->Update(aDeltaTime);
	}
	

	//LUA::ScriptSystem::GetInstance()->CallFunction("Update", { aDeltaTime });
	//LUA::ScriptSystem::GetInstance()->Update();

	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	if (myLevelComplete == false)
	{
		myLevel->Render();
	}

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

void InGameState::ReceiveNetworkMessage(const NetMessageAllClientsComplete& aMessage, const sockaddr_in&)
{
	DL_ASSERT_EXP(aMessage.myType == NetMessageAllClientsComplete::eType::LEVEL_COMPLETE
		|| aMessage.myType == NetMessageAllClientsComplete::eType::LEVEL_LOAD, "Unknown All clients complete message type");

	switch (aMessage.myType)
	{
	case NetMessageAllClientsComplete::eType::LEVEL_COMPLETE:
		myCanStartNextLevel = true;
		break;
	case NetMessageAllClientsComplete::eType::LEVEL_LOAD:
		myLevelComplete = false;
		break;
	}
}

void InGameState::ReceiveNetworkMessage(const NetMessageLevelComplete&, const sockaddr_in&)
{
	ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelComplete());
	ClientNetworkManager::GetInstance()->AllowSendWithoutSubscriber(true);
	SAFE_DELETE(myLevel);
	myLevelComplete = true;
}

void InGameState::ReceiveNetworkMessage(const NetMessageLoadLevel& aMessage, const sockaddr_in&)
{
	DL_ASSERT_EXP(myLevel == nullptr, "Level has to be nullptr here");
	SET_RUNTIME(false);
	myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(aMessage.myLevelID));
	ClientNetworkManager::GetInstance()->AllowSendWithoutSubscriber(false);
	ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelLoaded());
}

void InGameState::OnResize(int, int)
{
}