#include "stdafx.h"

#include <DL_Debug.h>
#include <NetworkMessageTypes.h>
#include <ServerNetworkManager.h>
#include <TimerManager.h>
#include <ServerGame.h>

volatile bool hasQuit = false;
static BOOL WINAPI HandleExit(DWORD aCtrlType)
{
	switch (aCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		ServerNetworkManager::Destroy();
		DL_Debug::Debug::Destroy();
		CU::TimerManager::Destroy();
		hasQuit = true;
		break;
	}
	return FALSE;
}

void main()
{
	CU::TimerManager::Create();

	DL_Debug::Debug::Create("NetworkLog.txt");
	ServerGame game;
	game.Init();
	ServerNetworkManager::Create();
	ServerNetworkManager::GetInstance()->StartNetwork();
	float deltaTime = 0.f;
	MSG winMessage;
	int a = SetConsoleCtrlHandler(HandleExit, TRUE);
	while (game.Update() == true)
	{
		if (hasQuit == true)break;
		CU::TimerManager::GetInstance()->Update();
		deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
		ServerNetworkManager::GetInstance()->Update(deltaTime);

		ServerNetworkManager::GetInstance()->MainIsDone();
		ServerNetworkManager::GetInstance()->WaitForReceieve();
	}
}