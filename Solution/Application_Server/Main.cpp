#include "stdafx.h"
#include <NetworkManager.h>
#include <DL_Debug.h>
void main()
{
	DL_Debug::Debug::Create("NetworkLog.txt");
	NetworkManager::Create(true);
	NetworkManager::GetInstance()->StartNetwork();
	while (true)
	{
		const CU::GrowingArray<Buffer>& messages = NetworkManager::GetInstance()->GetReceieveBuffer();


	}
	NetworkManager::Destroy();
	DL_Debug::Debug::Destroy();
}