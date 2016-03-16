#include "stdafx.h"

#include <PostMaster.h>
#include "ServerInGameState.h"
#include "ServerLobbyState.h"
#include "ServerNetworkManager.h"
#include "ServerStateStackProxy.h"
#include <NetMessageStartGame.h>
#include <iostream>
#include <PostMasterNetAddPlayerMessage.h>

ServerLobbyState::ServerLobbyState()
{
	myIsActiveState = false;
}

ServerLobbyState::~ServerLobbyState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
}

void ServerLobbyState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;

	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
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
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);

}

void ServerLobbyState::ReceiveMessage(const PostMasterNetRequestStartGameMessage&)
{
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageStartGame(myCurrentLevelID));

	ServerNetworkManager::GetInstance()->AllowNewConnections(false);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);

	myStateStack->PushMainState(new ServerInGameState(myCurrentLevelID));
}

void ServerLobbyState::ReceiveMessage(const PostMasterNetAddPlayerMessage& aMessage)
{
	//Broadcast join

	ServerNetworkManager::GetInstance()->CreateConnection(aMessage.myName, aMessage.myAddress);
}
