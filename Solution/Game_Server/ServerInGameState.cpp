#include "stdafx.h"
#include "ServerInGameState.h"
#include "ServerLevel.h"
#include "ServerLevelFactory.h"
#include "ServerLobbyState.h"
#include "ServerStateStackProxy.h"
#include <iostream>

ServerInGameState::ServerInGameState(int aLevelID)
	: myLevelID(aLevelID)
{
	myIsActiveState = false;
	myLevelFactory = new ServerLevelFactory("Data/Level/LI_level.xml");
}

ServerInGameState::~ServerInGameState()
{
	SAFE_DELETE(myLevel);
	SAFE_DELETE(myLevelFactory);
}

void ServerInGameState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;

	myLevel = static_cast<ServerLevel*>(myLevelFactory->LoadLevel(myLevelID));

	myIsActiveState = true;
}

void ServerInGameState::EndState()
{

}

const eStateStatus ServerInGameState::Update(const float aDeltaTime)
{
	myLevel->Update(aDeltaTime);

	return myStateStatus;
}

void ServerInGameState::ResumeState()
{
	myIsActiveState = true;
}