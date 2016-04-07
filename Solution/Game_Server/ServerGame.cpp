#include "stdafx.h"
#include "ServerGame.h"
#include "ServerStartState.h"
#include <TimerManager.h>
#include "ServerNetworkManager.h"
#include "NetworkMessageTypes.h"
#include "PostMaster.h"
#include <Utility.h>
#include <sstream>

ServerGame::ServerGame()
	: myTextUpdate(1.f)
	, myIsQuiting(false)
{
}

ServerGame::~ServerGame()
{
	myStateStack.Clear();
	ServerNetworkManager::Destroy();
	PostMaster::Destroy();
	myIsQuiting = true;
}

bool ServerGame::Init()
{
	PostMaster::Create();
	ServerNetworkManager::Create();
	ServerNetworkManager::GetInstance()->StartNetwork();
	myStateStack.PushMainState(new ServerStartState());
	return true;
}

bool ServerGame::Update()
{
	if (myIsQuiting == true)
	{
		return false;
	}
	myDeltaTime = myTimerManager->GetMasterTimer().GetTime().GetFrameTime();

	myTextUpdate -= myDeltaTime;
	if (myTextUpdate < 0.f)
	{
		std::stringstream ss;
		ss << ServerNetworkManager::GetInstance()->GetDataSent() << " kbyte/s";
		SetWindowText(Utility::GetConsoleHwnd(), ss.str().c_str());
		myTextUpdate = 1.f;
	}
	myTimerManager->Update();


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