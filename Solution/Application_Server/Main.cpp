#include "stdafx.h"
#include <NetworkManager.h>
#include <DL_Debug.h>
#include <NetworkMessageTypes.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessagePosition.h>
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
			{
				NetMessageConnectMessage onConnect;
				onConnect.UnPackMessage(buf.myData, buf.myLength);
				Utility::PrintEndl(onConnect.myName, Utility::eCOLOR::LIGHT_GREEN);

				NetworkManager::GetInstance()->GetNetworkHandle()->CreateConnection(onConnect.myName);
				break;
			}
			case eNetMessageType::POSITION:
			{
				NetMessagePosition pos;
				pos.UnPackMessage(buf.myData, buf.myLength);
				pos.PackMessage();
				NetworkManager::GetInstance()->AddMessage(pos.myStream);
				break;
			}
			
			}
		}
	}
	NetworkManager::Destroy();
	DL_Debug::Debug::Destroy();
}