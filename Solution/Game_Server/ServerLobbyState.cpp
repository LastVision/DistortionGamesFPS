#include "stdafx.h"
#include "ServerInGameState.h"
#include "ServerLobbyState.h"
#include "ServerStateStackProxy.h"
#include <iostream>

ServerLobbyState::ServerLobbyState()
{
	myIsActiveState = false;
}

ServerLobbyState::~ServerLobbyState()
{
}

void ServerLobbyState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;


	myIsActiveState = true;
}

void ServerLobbyState::EndState()
{
}

const eStateStatus ServerLobbyState::Update(const float aDeltaTime)
{
	aDeltaTime;
	myStateStack->PushMainState(new ServerInGameState());

	return myStateStatus;
}

void ServerLobbyState::ResumeState()
{
	myIsActiveState = true;
}