#include "stdafx.h"
#include "ServerStartState.h"
#include <NetMessageRequestConnect.h>
#include "ServerLobbyState.h"
#include "ServerStateStackProxy.h"
#include "ServerNetworkManager.h"

ServerStartState::ServerStartState()
{
}


ServerStartState::~ServerStartState()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
}

void ServerStartState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;
	myIsActiveState = true;
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	Utility::Printf("State: Start", eConsoleColor::AQUA_TEXT | eConsoleColor::DARK_GREEN_BACK);
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
	Utility::Printf("State: Start", eConsoleColor::AQUA_TEXT | eConsoleColor::DARK_GREEN_BACK);
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	myIsActiveState = true;
}

void ServerStartState::ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress)
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
	SET_RUNTIME(false);
	ServerNetworkManager::GetInstance()->CreateConnection(aMessage.myName, aSenderAddress);
	myStateStack->PushMainState(new ServerLobbyState(static_cast<eGameType>(aMessage.myGameType)));
}