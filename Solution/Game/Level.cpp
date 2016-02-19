#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkManager.h"
#include "NetworkMessageTypes.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
Level::Level()
	: myEntities(512)
{
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


	for (int i = 0; i < messages.Size(); ++i)
	{
		eNetMessageType type;
		memcpy(&type, &messages[i].myID, sizeof(char));
		if (type == eNetMessageType::ON_JOIN)
		{
			myOtherPlayer = EntityFactory::CreateEntity(eEntityType::PROP, "bullet", *myScene, CU::Vector3f(0.f, 0.f, 0.f));
			myOtherPlayer->AddToScene();
			messages.RemoveAll();
		}
	}

}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();
}