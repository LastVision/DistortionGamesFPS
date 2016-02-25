#include "stdafx.h"
#include <DL_Debug.h>
#include <NetworkMessageTypes.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessagePosition.h>
#include <ServerNetworkManager.h>
#include <Utility.h>
#include <TimerManager.h>
void main()
{

	CU::TimerManager::Create();

	DL_Debug::Debug::Create("NetworkLog.txt");
	
	ServerNetworkManager::Create();
	ServerNetworkManager::GetInstance()->StartNetwork();
	
	float deltaTime;
	while (true)
	{
		CU::TimerManager::GetInstance()->Update();
		deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
		ServerNetworkManager::GetInstance()->Update(deltaTime);
	}
	ServerNetworkManager::Destroy();
	DL_Debug::Debug::Destroy();
	CU::TimerManager::Destroy();
}