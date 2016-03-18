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

#include <NetMessageConnectReply.h>
#include <NetMessageDisconnect.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageOnDeath.h>
#include <NetMessageOnHit.h>
#include <NetMessageOnJoin.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageSetActive.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <CubeMapGenerator.h>
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
	, myInitDone(false)
{
	Prism::PhysicsInterface::Create(std::bind(&SharedLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2), false);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_REMOVE_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_DEATH, this);

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENEMY_ON_DEATH, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::PLAYER_ON_DEATH, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_ACTIVE, this);

	myScene = new Prism::Scene();
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

	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENEMY_ON_DEATH, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::PLAYER_ON_DEATH, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_ACTIVE, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);
}

void ClientLevel::Init()
{
	CreatePlayers();

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

void ClientLevel::SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint)
{
	myMinPoint = aMinPoint;
	myMaxPoint = aMaxPoint;
}

void ClientLevel::Update(const float aDeltaTime)
{
	if (myInitDone == false && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		myInitDone = true;
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelLoaded());

		SET_RUNTIME(false);
		myDeferredRenderer->GenerateSHData(myScene, myMinPoint, myMaxPoint);
		RESET_RUNTIME;
	}

	

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
	if (myInitDone == true)
	{
		myDeferredRenderer->Render(myScene);
		Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetComponent<InputComponent>()->GetCamera());
		//myScene->Render();
		//myDeferredRenderer->Render(myScene);
		myEmitterManager->RenderEmitters();
		myPlayer->GetComponent<FirstPersonRenderComponent>()->Render();
		//myPlayer->GetComponent<ShootingComponent>()->Render();
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in&)
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

void ClientLevel::ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in&)
{
	if (myActiveEntitiesMap.find(aMessage.myGID) == myActiveEntitiesMap.end())
	{
		DL_ASSERT("GID NOT FOUND IN CLIENT LEVEL!");
	}

	if (aMessage.myShouldActivate == true)
	{
		myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->AddToScene();
		myActiveEntitiesMap[aMessage.myGID]->AddToScene();
	}
	else
	{
		myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->RemoveFromScene();
		myActiveEntitiesMap[aMessage.myGID]->RemoveFromScene();
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

void ClientLevel::CreatePlayers()
{
	myPlayer = EntityFactory::GetInstance()->CreateEntity(ClientNetworkManager::GetInstance()->GetGID(), eEntityType::UNIT, "localplayer", myScene, true, CU::Vector3<float>(0.f, 2.f, 10.f));

	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	for each (const OtherClients& client in ClientNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		newPlayer->AddToScene();
		newPlayer->Reset();
		myPlayers.Add(newPlayer);
	}
}