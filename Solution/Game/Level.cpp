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
Level::Level()
	: myEntities(512)
{
	connected = false;
	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());
	myTempPosition = { 835.f, 0.f, -1000.f };
}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
}

void Level::AddEntity(Entity* aEntity)
{
	myEntities.Add(aEntity);
}

void Level::Update(const float aDeltaTime)
{
	NetworkManager::GetInstance()->Swap(true); 
	myPlayer->Update(aDeltaTime);
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
			break;
		}
		
		case eNetMessageType::ON_JOIN:
			connected = true;
			break;
		case eNetMessageType::POSITION:
		{
			NetMessagePosition pos;
			pos.UnPackMessage(buf.myData, buf.myLength);
			if (pos.mySenderID != NetworkManager::GetInstance()->GetNetworkID())
			{
				myTempPosition = pos.myPosition;
			}
			break;
		}
		}
	}

	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetCamera());


}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();


	if (connected == true)
	{
		DEBUG_PRINT("Player 2 has joined!");
		Prism::DebugDrawer::GetInstance()->RenderBox(myTempPosition, eColorDebug::BLUE, 300.f);
	}
	else
	{
		DEBUG_PRINT("Nobody is here!");
	}
}