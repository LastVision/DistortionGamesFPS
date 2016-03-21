#include "stdafx.h"
#include "ServerGame.h"
#include "ServerLobbyState.h"
#include <TimerManager.h>
#include "ServerNetworkManager.h"
#include "NetworkMessageTypes.h"
#include "PostMaster.h"

ServerGame::ServerGame()
{
}

ServerGame::~ServerGame()
{
	ServerNetworkManager::Destroy();
	myStateStack.Clear();
	PostMaster::Destroy();
}

bool ServerGame::Init()
{
	PostMaster::Create();
	ServerNetworkManager::Create();
	ServerNetworkManager::GetInstance()->StartNetwork();
	myStateStack.PushMainState(new ServerLobbyState());
	return true;
}

bool ServerGame::Update()
{
	myTimerManager->Update();

	myDeltaTime = myTimerManager->GetMasterTimer().GetTime().GetFrameTime();

	if (myStateStack.UpdateCurrentState(myDeltaTime) == false)
	{
		return false;
	}
	ServerNetworkManager::GetInstance()->Update(myDeltaTime);

	myTimerManager->CapFrameRate(60.f);

	ServerNetworkManager::GetInstance()->MainIsDone();
	ServerNetworkManager::GetInstance()->WaitForReceieve();
	return true;
}