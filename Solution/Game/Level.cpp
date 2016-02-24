#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkManager.h"
#include "NetworkMessageTypes.h"
#include "NetworkMessageTypes.h"
#include "NetMessagePosition.h"
#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
#include <NetMessageConnectMessage.h>
#include <PhysicsInterface.h>
#include <ClientInterface.h>
#include "NetMessageOnJoin.h"

Level::Level()
	: myEntities(512)
	, myClients(16)
{
	Prism::PhysicsInterface::Create();
	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());
	//myTempPosition = { 835.f, 0.f, -1000.f };
}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	Prism::PhysicsInterface::Destroy();
}

void Level::AddEntity(Entity* aEntity)
{
	myEntities.Add(aEntity);
}

void Level::Update(const float aDeltaTime)
{
	NetworkManager::GetInstance()->Swap(true);
	myPlayer->Update(aDeltaTime);

	for (int i = 0; i < myEntities.Size(); i++)
	{
		myEntities[i]->Update(aDeltaTime);
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_C) == true)
	{
		NetworkManager::GetInstance()->ConnectToServer();
		//NetworkManager::GetInstance()->ConnectToServer("172.22.53.245"); //LinusS IP
	}

	const CU::GrowingArray<Buffer>& messages = NetworkManager::GetInstance()->GetReceieveBuffer();
	for (Buffer buf : messages)
	{
		eNetMessageType type = NetworkManager::GetInstance()->ReadType(&buf.myData[0]);

		switch (type)
		{
		case eNetMessageType::ON_CONNECT:
		{
			NetMessageConnectMessage connect;
			connect.UnPackMessage(buf.myData, buf.myLength);
			NetworkManager::GetInstance()->SetNetworkID(connect.myServerID);
			const CU::GrowingArray<OtherClients>& clients = static_cast<ClientInterface*>(NetworkManager::GetInstance()->GetNetworkHandle())->GetClientList();
			for (OtherClients c : clients)
			{
				myClients.Add(c);
			}
			break;
		}

		case eNetMessageType::ON_JOIN:
		{
			NetMessageOnJoin onJoin;
			onJoin.UnPackMessage(buf.myData, buf.myLength);
			OtherClients c;
			c.myID = onJoin.mySenderID;
			myClients.Add(c);
			break;
		}
		case eNetMessageType::POSITION:
		{
			NetMessagePosition pos;
			pos.UnPackMessage(buf.myData, buf.myLength);
			if (pos.mySenderID != NetworkManager::GetInstance()->GetNetworkID())
			{
				for (OtherClients& c : myClients)
				{
					if (pos.mySenderID == c.myID)
					{
						c.myPosition = pos.myPosition;
					}
				}

			}
			break;
		}
		}
	}

	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetCamera());

	Prism::PhysicsInterface::GetInstance()->Update();
}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();

	for (int i = 0; i < myClients.Size(); i++)
	{
		Prism::DebugDrawer::GetInstance()->RenderBox(myClients[i].myPosition, eColorDebug::BLUE, 1.f);
	}


}