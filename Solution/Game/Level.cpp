#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkManager.h"
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
Level::Level()
	: myEntities(512)
{
	connected = false;
	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());
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
	myPlayer->Update(aDeltaTime);
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_C) == true)
	{
		NetworkManager::GetInstance()->ConnectToServer();
	}

	CU::GrowingArray<NetMessage> messages = NetworkManager::GetInstance()->GetBuffer();
	if (connected == true)
	{
		DEBUG_PRINT("Player 2 has joined!");
	}
	else
	{
		DEBUG_PRINT("Nobody is here!");
	}

	for (int i = 0; i < messages.Size(); ++i)
	{

		if (messages[i].myID == '\x2')
		{
			connected = true;
			messages.RemoveAll();
		}
	}

}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();
}