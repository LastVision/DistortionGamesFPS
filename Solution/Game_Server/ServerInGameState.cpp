#include "stdafx.h"
#include "ServerInGameState.h"

ServerInGameState::ServerInGameState()
{
	myIsActiveState = false;
}

ServerInGameState::~ServerInGameState()
{
}

void ServerInGameState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;


	myIsActiveState = true;
}

void ServerInGameState::EndState()
{

}

const eStateStatus ServerInGameState::Update(const float aDeltaTime)
{
	return myStateStatus;
}

void ServerInGameState::ResumeState()
{
	myIsActiveState = true;
}