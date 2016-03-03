#include "stdafx.h"
#include "ServerInGameState.h"
#include "ServerLevel.h"
#include "ServerLobbyState.h"
#include "ServerStateStackProxy.h"
#include <iostream>

ServerInGameState::ServerInGameState()
{
	myIsActiveState = false;
}

ServerInGameState::~ServerInGameState()
{
	SAFE_DELETE(myCurrentLevel);
}

void ServerInGameState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;

	myCurrentLevel = new ServerLevel();

	myIsActiveState = true;
}

void ServerInGameState::EndState()
{

}

const eStateStatus ServerInGameState::Update(const float aDeltaTime)
{
	myCurrentLevel->Update(aDeltaTime);

	return myStateStatus;
}

void ServerInGameState::ResumeState()
{
	myIsActiveState = true;
}