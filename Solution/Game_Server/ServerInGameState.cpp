#include "stdafx.h"

#include <CommonHelper.h>
#include <NetMessageAllClientsComplete.h>
#include <NetMessageDisconnect.h>
#include <NetMessageLevelComplete.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageLoadLevel.h>
#include <MurmurHash3.h>
#include "ServerInGameState.h"
#include "ServerLevel.h"
#include "ServerLevelFactory.h"
#include "ServerLobbyState.h"
#include "ServerNetworkManager.h"
#include "ServerStateStackProxy.h"
#include <iostream>
#include <PhysicsInterface.h>

ServerInGameState::ServerInGameState(int aLevelID, unsigned int aLevelHashValue)
	: myLevelID(aLevelID)
	, myState(eInGameStates::LEVEL_LOAD)
	, myRespondedClients(16)
	, myLevelHashValue(aLevelHashValue)
	, myGameComplete(false)
{
	myIsActiveState = false;
	myLevelFactory = new ServerLevelFactory("Data/Level/LI_level.xml");
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_LOADED, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DISCONNECT, this);
}

ServerInGameState::~ServerInGameState()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_LOADED, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DISCONNECT, this);
	SAFE_DELETE(myLevel);
	SAFE_DELETE(myLevelFactory);
}

void ServerInGameState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;
	ServerNetworkManager::GetInstance()->StopSendMessages(true);
	ServerNetworkManager::GetInstance()->StopPing(true);
	myLevel = static_cast<ServerLevel*>(myLevelFactory->LoadLevel(myLevelID));
	ServerNetworkManager::GetInstance()->StopSendMessages(false);

	myIsActiveState = true;

	Utility::Printf("State: InGame", eConsoleColor::AQUA_TEXT);
}

void ServerInGameState::EndState()
{

}

const eStateStatus ServerInGameState::Update(const float aDeltaTime)
{
	switch (myState)
	{
	case eInGameStates::LEVEL_UPDATE:
		ServerNetworkManager::GetInstance()->StopPing(false);
		LevelUpdate(aDeltaTime);
		break;
	case eInGameStates::LEVEL_COMPLETE:
		// only waiting for messages from clients
		break;
	case eInGameStates::LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED:
		// only waiting for messages from clients
		break;
	case eInGameStates::LEVEL_LOAD:
		if (Prism::PhysicsInterface::GetInstance()->GetInitDone() 
			&& ServerNetworkManager::GetInstance()->ListContainsAllClients(myRespondedClients) == true)
		{
			myRespondedClients.RemoveAll();
			myState = eInGameStates::LEVEL_UPDATE;
			ServerNetworkManager::GetInstance()->AddMessage(NetMessageAllClientsComplete(NetMessageAllClientsComplete::eType::LEVEL_LOAD));
		}
		// only waiting for messages from clients
		break;
	default:
		DL_ASSERT("Unknown state type");
	}

	return myStateStatus;
}

void ServerInGameState::ResumeState()
{
	Utility::Printf("State: InGame", eConsoleColor::AQUA_TEXT);
	myIsActiveState = true;
}

void ServerInGameState::ReceiveNetworkMessage(const NetMessageLevelComplete& aMessage, const sockaddr_in&)
{
	myRespondedClients.Add(aMessage.mySenderID);
	if (ServerNetworkManager::GetInstance()->ListContainsAllClients(myRespondedClients) == true)
	{
		myRespondedClients.RemoveAll();
		myState = eInGameStates::LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED;
		SET_RUNTIME(false);
		SAFE_DELETE(myLevel);
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageAllClientsComplete(NetMessageAllClientsComplete::eType::LEVEL_COMPLETE));
		ServerNetworkManager::GetInstance()->StopSendMessages(true);
		ServerNetworkManager::GetInstance()->StopPing(true);
		myLevelHashValue = Hash(CU::ReadFileToString(myLevelFactory->GetLevelPath(myLevelID)).c_str());
		myLevel = static_cast<ServerLevel*>(myLevelFactory->LoadLevel(myLevelID));
		ServerNetworkManager::GetInstance()->StopSendMessages(false);
	}
}

void ServerInGameState::ReceiveNetworkMessage(const NetMessageRequestStartLevel&, const sockaddr_in&)
{
	//DL_ASSERT_EXP(myState == eInGameStates::LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED, "Wrong state for Request Start Level message.");
	//Can't assert and support space pressed twice at the same time.
	if (myState == eInGameStates::LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED)
	{
		myState = eInGameStates::LEVEL_LOAD;
		
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageLoadLevel(myLevelID, myLevelHashValue));
	}
}

void ServerInGameState::ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in&)
{
	myRespondedClients.Add(aMessage.mySenderID);
	
}

void ServerInGameState::ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	if (aMessage.mySenderID == 1 || ServerNetworkManager::GetInstance()->GetClients().Size() <= 0)
	{
		ServerNetworkManager::GetInstance()->DisconnectAll();
		myStateStatus = eStateStatus::POP_MAIN_STATE;
	}
}

void ServerInGameState::LevelUpdate(float aDeltaTime)
{
	int nextLevel;
	if (myLevel->ChangeLevel(nextLevel) == true)
	{
		if (nextLevel == 0 && myLevelID != 0)
		{
			myGameComplete = true;
			myStateStatus = eStateStatus::POP_MAIN_STATE;
		}
		myState = eInGameStates::LEVEL_COMPLETE;
		myLevelID = nextLevel;
		myRespondedClients.RemoveAll();
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageLevelComplete(false));
	}
	else
	{
		myLevel->Update(aDeltaTime, false);
	}
}