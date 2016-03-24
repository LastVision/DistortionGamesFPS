#include "stdafx.h"

#include <PostMaster.h>
#include "ServerInGameState.h"
#include "ServerLobbyState.h"
#include "ServerNetworkManager.h"
#include "ServerStateStackProxy.h"
#include <NetMessageLoadLevel.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageSetLevel.h>
#include <NetMessageRequestStartLevel.h>


ServerLobbyState::ServerLobbyState()
{
	myIsActiveState = false;
}

ServerLobbyState::~ServerLobbyState()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);

}

void ServerLobbyState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;

	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	myCurrentLevelID = 0;
	myIsActiveState = true;
	
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);
}

void ServerLobbyState::EndState()
{
}

const eStateStatus ServerLobbyState::Update(const float aDeltaTime)
{
	aDeltaTime;

	return myStateStatus;
}

void ServerLobbyState::ResumeState()
{
	myIsActiveState = true;
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in&)
{
	myCurrentLevelID = aMessage.myLevelID;
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetLevel(myCurrentLevelID));
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageRequestStartLevel&, const sockaddr_in&)
{
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageLoadLevel(myCurrentLevelID));

	ServerNetworkManager::GetInstance()->AllowNewConnections(false);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);

	SET_RUNTIME(false);
	myStateStack->PushMainState(new ServerInGameState(myCurrentLevelID));
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress)
{
	//Broadcast join
	ServerNetworkManager::GetInstance()->CreateConnection(aMessage.myName, aSenderAddress);
}
