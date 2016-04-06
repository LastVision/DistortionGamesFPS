#include "stdafx.h"

#include <CommonHelper.h>
#include <NetMessageAllClientsComplete.h>
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

}

ServerInGameState::~ServerInGameState()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_LOADED, this);
	SAFE_DELETE(myLevel);
	SAFE_DELETE(myLevelFactory);
}

void ServerInGameState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;
	ServerNetworkManager::GetInstance()->StopSendMessages(true);
	myLevel = static_cast<ServerLevel*>(myLevelFactory->LoadLevel(myLevelID));
	ServerNetworkManager::GetInstance()->StopSendMessages(false);

	myIsActiveState = true;
}

void ServerInGameState::EndState()
{

}

const eStateStatus ServerInGameState::Update(const float aDeltaTime)
{
	switch (myState)
	{
	case eInGameStates::LEVEL_UPDATE:
		LevelUpdate(aDeltaTime);
		break;
	case eInGameStates::LEVEL_COMPLETE:
		// only waiting for messages from clients
		break;
	case eInGameStates::LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED:
		// only waiting for messages from clients
		break;
	case eInGameStates::LEVEL_LOAD:
		// only waiting for messages from clients
		break;
	default:
		DL_ASSERT("Unknown state type");
	}

	return myStateStatus;
}

void ServerInGameState::ResumeState()
{
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
	if (ServerNetworkManager::GetInstance()->ListContainsAllClients(myRespondedClients) == true)
	{
		myRespondedClients.RemoveAll();
		myState = eInGameStates::LEVEL_UPDATE;
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageAllClientsComplete(NetMessageAllClientsComplete::eType::LEVEL_LOAD));
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
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageLevelComplete());
	}
	else
	{
		myLevel->Update(aDeltaTime);
	}
}