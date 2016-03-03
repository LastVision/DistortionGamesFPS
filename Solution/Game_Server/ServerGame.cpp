#include "stdafx.h"
#include "ServerGame.h"
#include "ServerInGameState.h"
#include <TimerManager.h>

ServerGame::ServerGame()
{
}

ServerGame::~ServerGame()
{
	myStateStack.Clear();
}

bool ServerGame::Init()
{
	myStateStack.PushMainState(new ServerInGameState());
	return true;
}

bool ServerGame::Update()
{
	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();

	if (myStateStack.UpdateCurrentState(deltaTime) == false)
	{
		return false;
	}
	return true;
}