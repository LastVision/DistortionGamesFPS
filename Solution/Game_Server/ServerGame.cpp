#include "stdafx.h"
#include "ServerGame.h"
#include "ServerInGameState.h"
#include <TimerManager.h>
#include "ServerNetworkManager.h"
#include "NetworkMessageTypes.h"

ServerGame::ServerGame()
{
}

ServerGame::~ServerGame()
{
	myStateStack.Clear();
	ServerNetworkManager::Destroy();
	CU::TimerManager::Destroy();

}

bool ServerGame::Init()
{
	CU::TimerManager::Create();
	ServerNetworkManager::Create();
	ServerNetworkManager::GetInstance()->StartNetwork();
	myStateStack.PushMainState(new ServerInGameState());
	return true;
}

bool ServerGame::Update()
{
	CU::TimerManager::GetInstance()->Update();

	myDeltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();

	if (myStateStack.UpdateCurrentState(myDeltaTime) == false)
	{
		return false;
	}
	ServerNetworkManager::GetInstance()->Update(myDeltaTime);


	ServerNetworkManager::GetInstance()->MainIsDone();
	ServerNetworkManager::GetInstance()->WaitForReceieve();
	return true;
}