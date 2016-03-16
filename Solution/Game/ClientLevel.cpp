#include "stdafx.h"
#include <Instance.h>
#include <AudioInterface.h>

#include "ClientLevel.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include <InputComponent.h>
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include <EntityData.h>
#include <FirstPersonRenderComponent.h>
#include "GameEnum.h"
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>

#include <NetMessageOnJoin.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageDisconnect.h>
#include <NetMessageOnHit.h>

#include <PostMasterNetAddPlayerMessage.h>
#include <PostMasterNetRemovePlayer.h>
#include <PostMasterNetAddEnemyMessage.h>
#include <PostMasterNetOnDeathMessage.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <PointLight.h>

#include <PhysicsInterface.h>
#include <PostMaster.h>

#include "EmitterManager.h"
#include <EmitterMessage.h>

#include <NetworkComponent.h>

ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
{
	//myBulletExplosions.Init(8);
	//Prism::PhysicsInterface::Destroy();
	//Prism::PhysicsInterface::GetInstance()->RayCast({ 0, 0, 0 }, { 0, 1, 0 }, 10.f);
	//Prism::PhysicsInterface::GetInstance()->Update();
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_REMOVE_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_DEATH, this);

	myScene = new Prism::Scene();
	EntityData data;
	data.myInputData.myExistsInEntity = true;
	//myPlayer = new Entity(data, myScene, true, CU::Vector3<float>(), CU::Vector3<float>(), CU::Vector3<float>(1.f, 1.f, 1.f));
	myPlayer = EntityFactory::GetInstance()->CreateEntity(UINT32_MAX, eEntityType::UNIT, "localplayer", myScene, true, CU::Vector3<float>());
	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	//myTempPosition = { 835.f, 0.f, -1000.f };

	Prism::ModelLoader::GetInstance()->Pause();
	myDeferredRenderer = new Prism::DeferredRenderer();

	myEmitterManager = new EmitterManager(*myPlayer->GetComponent<InputComponent>()->GetCamera());
	Prism::ModelLoader::GetInstance()->UnPause();
	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlaySecondLayer", 0);
}

ClientLevel::~ClientLevel()
{
	if (ClientNetworkManager::GetInstance()->GetGID() > 0)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
	}
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif

	SAFE_DELETE(myEmitterManager);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_REMOVE_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_DEATH, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);
}

void ClientLevel::Update(const float aDeltaTime)
{
	SharedLevel::Update(aDeltaTime);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->UpdateCoOpPositions(myPlayers);
	myPlayer->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_T))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(eNetMessageType::PLAYER_ON_HIT, 5.f, 17));
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_J))
	{
		for (unsigned int i = 0; i < 8; ++i)
		{
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(eNetMessageType::PLAYER_ON_HIT, 5.f, i));
		}
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_I))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
	}

	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);

	//for (int i = 0; i < myPlayers.Size(); ++i)
	//{
	//	CU::Vector3f position = ClientNetworkManager::GetInstance()->GetClients()[i].myPosition;
	//	myPlayers[i]->SetPosition(position);
	//}

	DebugMusic();

	Prism::PhysicsInterface::GetInstance()->EndFrame();

	ClientNetworkManager::GetInstance()->Update(aDeltaTime);

}

void ClientLevel::Render()
{
	myDeferredRenderer->Render(myScene);
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetComponent<InputComponent>()->GetCamera());
	//myScene->Render();
	//myDeferredRenderer->Render(myScene);
	myEmitterManager->RenderEmitters();
	myPlayer->GetComponent<FirstPersonRenderComponent>()->Render();
}

void ClientLevel::ReceiveMessage(const PostMasterNetAddPlayerMessage& aMessage)
{
	if (aMessage.myGID == ClientNetworkManager::GetInstance()->GetGID())
	{
		myPlayer->SetGID(aMessage.myGID);
	}
	else
	{
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(aMessage.myGID, eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
	newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
	newPlayer->AddToScene();
	newPlayer->Reset();
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}
}

void ClientLevel::ReceiveMessage(const PostMasterNetRemovePlayerMessage& aMessage)
{
	DL_ASSERT_EXP(aMessage.myGID != 0, "Can't remove server (id 0).");
	for (Entity* e : myPlayers)
	{
		if (e->GetGID() == aMessage.myGID)
		{
			e->RemoveFromScene();
		}
	}
}

void ClientLevel::ReceiveMessage(const PostMasterNetAddEnemyMessage& aMessage)
{
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newEnemy = EntityFactory::CreateEntity(aMessage.myGID, eEntityType::UNIT, "gundroid", myScene, true, aMessage.myPosition);

	newEnemy->AddToScene();
	newEnemy->Reset();

	myActiveEnemies.Add(newEnemy);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}

void ClientLevel::ReceiveMessage(const PostMasterNetOnDeathMessage& aMessage)
{
	DL_ASSERT_EXP(aMessage.myGID != 0, "Can't kill server (id 0).");

	for (Entity* e : myActiveEnemies)
	{
		if (e->GetGID() == aMessage.myGID)
		{
			e->Kill();
		}
	}
}

void ClientLevel::AddLight(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
}

void ClientLevel::DebugMusic()
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInBackground", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_5) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutBackground", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_6) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInFirstLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_7) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutFirstLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_8) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInSecondLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_9) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutSecondLayer", 0);
	}
}