#include "stdafx.h"

#include <DL_Debug.h>
#include <TimerManager.h>
#include <ServerGame.h>
#include <assert.h>
#include <TimerManager.h>
#include <ServerNetworkManager.h>

ServerGame* globalServerGame;

static BOOL WINAPI HandleExit(DWORD aCtrlType)
{
	switch (aCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		DL_DEBUG("Exit server");
		DL_Debug::Debug::Destroy();
		delete globalServerGame;
		globalServerGame = nullptr;
		return TRUE;
	}
	return FALSE;
}

void main()
{
	int error = SetConsoleCtrlHandler(HandleExit, TRUE);
	assert(error != 0 && "Failed to set console handler, can't exit clean. Nothing is created!");

	DL_Debug::Debug::Create("NetworkLog.txt");
	globalServerGame = new ServerGame();
	globalServerGame->Init();
	while (globalServerGame->Update() == true);

	DL_DEBUG("Exit server");
	DL_Debug::Debug::Destroy();
	delete globalServerGame;
	globalServerGame = nullptr;
}