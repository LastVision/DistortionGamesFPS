#include "stdafx.h"
#include <NetworkManager.h>
#include <DL_Debug.h>
#include <NetworkMessageTypes.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <Utility.h>
void main()
{
	DL_Debug::Debug::Create("NetworkLog.txt");
	NetworkManager::Create(true);
	NetworkManager::GetInstance()->StartNetwork();
	while (true)
	{
		NetworkManager::GetInstance()->Swap(true);
		const CU::GrowingArray<Buffer>& messages = NetworkManager::GetInstance()->GetReceieveBuffer();
		for (Buffer buf : messages)
		{
			eNetMessageType type = NetworkManager::GetInstance()->ReadType(&buf.myData[0]);
			switch (type)
			{
			case eNetMessageType::ON_CONNECT:
				NetMessageConnectMessage onConnect;
				onConnect.UnPackMessage(buf.myData, buf.myLength);
				Utility::PrintEndl(onConnect.myName, Utility::eCOLOR::LIGHT_GREEN);

				NetworkManager::GetInstance()->GetNetworkHandle()->CreateConnection(onConnect.myName);

				//NetMessageOnJoin toPack; //he's back
				//toPack.Init();
				//toPack.PackMessage();
				//NetworkManager::GetInstance()->AddMessage(toPack.myStream);


				break;
			}


		}

	}
	NetworkManager::Destroy();
	DL_Debug::Debug::Destroy();
}