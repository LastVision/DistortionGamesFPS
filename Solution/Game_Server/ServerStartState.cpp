#include "stdafx.h"
#include "ServerStartState.h"
#include <NetMessageStartLobby.h>

ServerStartState::ServerStartState()
{
}


ServerStartState::~ServerStartState()
{
}

void ServerStartState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;
	myIsActiveState = true;
}

void ServerStartState::EndState()
{

}

const eStateStatus ServerStartState::Update(const float)
{

	return myStateStatus;
}

void ServerStartState::ResumeState()
{
	myIsActiveState = true;
}

void ServerStartState::ReceiveNetworkMessage(const NetMessageStartLobby& aMessage, const sockaddr_in&)
{
	aMessage;
}