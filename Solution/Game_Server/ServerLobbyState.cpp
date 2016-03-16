#include "stdafx.h"

#include <PostMaster.h>
#include "ServerInGameState.h"
#include "ServerLobbyState.h"
#include "ServerNetworkManager.h"
#include "ServerStateStackProxy.h"
#include <NetMessageStartGame.h>
#include <iostream>

ServerLobbyState::ServerLobbyState()
{
	myIsActiveState = false;
}

ServerLobbyState::~ServerLobbyState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
}

void ServerLobbyState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;

	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
	myCurrentLevelID = 0;
	myIsActiveState = true;
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
}

void ServerLobbyState::ReceiveMessage(const PostMasterNetRequestStartGameMessage&)
{
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageStartGame(myCurrentLevelID));


	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_REQUEST_START_GAME, this);
	myStateStack->PushMainState(new ServerInGameState(myCurrentLevelID));
}