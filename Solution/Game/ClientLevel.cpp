#include "stdafx.h"
#include <Instance.h>
#include <AudioInterface.h>
#include <AnimationComponent.h>
#include "ClientLevel.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include <InputComponent.h>
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include <CollisionNote.h>
#include "EntityFactory.h"
#include "Entity.h"
#include <EntityData.h>
#include <FirstPersonRenderComponent.h>
#include "GameEnum.h"
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>

#include <NetMessageConnectReply.h>
#include <NetMessageDisconnect.h>
#include <NetMessageEntityState.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageOnDeath.h>
#include <NetMessageHealthPack.h>
#include <NetMessageOnJoin.h>
#include <NetMessageExplosion.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageShootGrenade.h>
#include <NetMessageSetActive.h>
#include <NetMessageEnemyShooting.h>
#include <NetMessageRayCastRequest.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <Renderer.h>
#include <CubeMapGenerator.h>
#include <PointLight.h>
#include <SpotLight.h>
#include <GrenadeComponent.h>

#include <PhysicsInterface.h>
#include <PostMaster.h>
#include <ShootingComponent.h>
#include <TriggerComponent.h>
#include <UpgradeComponent.h>
#include <UpgradeNote.h>
#include "EmitterManager.h"
#include <EmitterMessage.h>

#include <NetworkComponent.h>
#include "ClientProjectileManager.h"
#include "ClientUnitManager.h"
#include <NetMessageActivateSpawnpoint.h>
#include "TextEventManager.h"
#include <TextProxy.h>
#include <RoomManager.h>
#include <ParticleEmitterInstance.h>
#include <Room.h>
ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
	, mySpotLights(64)
	, myInitDone(false)
	, myForceStrengthPistol(0.f)
	, myForceStrengthShotgun(0.f)
	, myWorldTexts(64)
{
	Prism::PhysicsInterface::Create(std::bind(&ClientLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), false);

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENEMY_SHOOTING, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SHOOT_GRENADE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::EXPLOSION, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::RAY_CAST_REQUEST, this);

	ClientProjectileManager::Create();
	ClientUnitManager::Create();
	myScene = new Prism::Scene();

	myOtherClientRaycastHandlerPistol = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleOtherClientRayCastPistol(aComponent, aDirection, aHitPosition, aHitNormal);
	};

	myOtherClientRaycastHandlerShotgun = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleOtherClientRayCastShotgun(aComponent, aDirection, aHitPosition, aHitNormal);
	};
	myEmitterManager = new EmitterManager();

	myTestText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true, false);

	myTestText->SetOffset({ 0.f, 1.f, 10.f });

}

ClientLevel::~ClientLevel()
{
	for (int i = 0; i < myWorldTexts.Size(); ++i)
	{
		SAFE_DELETE(myWorldTexts[i].myProxy);
	}
	SAFE_DELETE(myTestText);
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif

	SAFE_DELETE(myEmitterManager);
	ClientProjectileManager::Destroy();
	ClientUnitManager::Destroy();
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENTITY_STATE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENEMY_SHOOTING, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SHOOT_GRENADE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::EXPLOSION, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::RAY_CAST_REQUEST, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	mySpotLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);
	SAFE_DELETE(myFullscreenRenderer);
	SAFE_DELETE(myTextManager);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);
}

void ClientLevel::Init(const std::string& aWeaponSettingsPath)
{
	CreatePlayers();
	myForceStrengthPistol = myPlayer->GetComponent<ShootingComponent>()->GetWeaponForceStrength(eWeaponType::PISTOL);
	myForceStrengthShotgun = myPlayer->GetComponent<ShootingComponent>()->GetWeaponForceStrength(eWeaponType::SHOTGUN);

	Prism::ModelLoader::GetInstance()->Pause();
	myDeferredRenderer = new Prism::DeferredRenderer();
	myDeferredRenderer->SetCubeMap("Data/Resource/Texture/CubeMap/church_horizontal_cross_cube_specular_pow2.dds");

	myFullscreenRenderer = new Prism::Renderer();


	Prism::ModelLoader::GetInstance()->UnPause();
	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayAll", 0);
	ClientProjectileManager::GetInstance()->CreateBullets(myScene);
	ClientProjectileManager::GetInstance()->CreateGrenades(myScene);
	ClientProjectileManager::GetInstance()->CreateExplosions();

	myTextManager = new TextEventManager(myPlayer->GetComponent<InputComponent>()->GetCamera());
	myEmitterManager->Initiate(myPlayer->GetComponent<InputComponent>()->GetCamera());

	myPlayer->GetComponent<ShootingComponent>()->ReadXMLSettings(aWeaponSettingsPath);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->Init();
}

void ClientLevel::SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint)
{
	myMinPoint = aMinPoint;
	myMaxPoint = aMaxPoint;
}

void ClientLevel::SetName(const std::string& aName)
{
	myName = aName;
}

void ClientLevel::Update(const float aDeltaTime)
{
	if (myInitDone == false && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		myInitDone = true;
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelLoaded());

		SET_RUNTIME(false);
		myDeferredRenderer->GenerateSHData(myScene, myMinPoint, myMaxPoint, myName);
		RESET_RUNTIME;
	}
	if (myInitDone == false)
	{
		return;
	}
	ClientProjectileManager::GetInstance()->Update(aDeltaTime);
	ClientUnitManager::GetInstance()->Update(aDeltaTime);
	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_U))
	//{
	//	myActiveEnemies.GetLast()->SetState(eEntityState::WALK);
	//}
	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_Y))
	//{
	//	myActiveEnemies.GetLast()->SetState(eEntityState::ATTACK);
	//}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_B) == true)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageActivateSpawnpoint(17));
	}

	SharedLevel::Update(aDeltaTime);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->UpdateCoOpPositions(myPlayers);
	myPlayer->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());
	myTextManager->Update(aDeltaTime);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_I))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
	}

	unsigned long long ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(int(ms));
	DEBUG_PRINT(kbs);

	//for (int i = 0; i < myPlayers.Size(); ++i)
	//{
	//	CU::Vector3f position = ClientNetworkManager::GetInstance()->GetClients()[i].myPosition;
	//	myPlayers[i]->SetPosition(position);
	//}

	DebugMusic();

	if (Prism::ModelLoader::GetInstance()->IsLoading() == false && myTestText->IsLoaded() == true)
	{
		static float totalTime = 0.f;
		totalTime += aDeltaTime;
		myTestText->SetText("Press W to walk forward.");
		myTestText->SetColor({ 0.f, abs(cos(totalTime)), abs(cos(-totalTime)), 1.f });
		myTestText->SetOffset({ 0.f, 1.f + (cos(totalTime * 2.f) * 0.5f), 10.f });
		//myTestText->Rotate3dText((cos(totalTime * 2.f) * 0.5f));

		for (int i = 0; i < myWorldTexts.Size(); ++i)
		{
			myWorldTexts[i].myProxy->SetText(myWorldTexts[i].myText);
		}
	}


	Prism::PhysicsInterface::GetInstance()->EndFrame();

	ClientNetworkManager::GetInstance()->Update(aDeltaTime);


}

void ClientLevel::Render()
{
	if (myInitDone == true)
	{
		myDeferredRenderer->Render(myScene);

		myFullscreenRenderer->Render(myDeferredRenderer->GetFinishedTexture(), myDeferredRenderer->GetEmissiveTexture(), myDeferredRenderer->GetDepthStencilTexture(), Prism::ePostProcessing::BLOOM);

		Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetComponent<InputComponent>()->GetCamera());

		//myScene->Render();
		//myDeferredRenderer->Render(myScene);
		if (myScene->GetRoomManager()->GetPlayerRoom()->GetEmitter() != nullptr)
		{
			myScene->GetRoomManager()->GetPlayerRoom()->GetEmitter()->SetShouldRender(true);
		}
		if (myScene->GetRoomManager()->GetPreviousPlayerRoom() != nullptr)
		{
			if (myScene->GetRoomManager()->GetPreviousPlayerRoom()->GetEmitter() != nullptr)
			{
				myScene->GetRoomManager()->GetPreviousPlayerRoom()->GetEmitter()->SetShouldAlwaysShow(false);
			}
		}
		myEmitterManager->RenderEmitters();


		myPlayer->GetComponent<FirstPersonRenderComponent>()->Render();
		//myPlayer->GetComponent<ShootingComponent>()->Render();

		myTextManager->Render();
		myTestText->Render(myScene->GetCamera());
		for (int i = 0; i < myWorldTexts.Size(); ++i)
		{
			myWorldTexts[i].myProxy->Render(myScene->GetCamera());
		}
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
	bool useEntityMap = true;

	if (aMessage.myGID == myPlayer->GetGID())
	{
		if (aMessage.myShouldActivate == true)
		{
			myPlayer->GetComponent<PhysicsComponent>()->Wake();
		}
		else
		{
			myPlayer->GetComponent<PhysicsComponent>()->Sleep();
		}
		return;
	}

	if (myActiveEntitiesMap.find(aMessage.myGID) == myActiveEntitiesMap.end())
	{
		useEntityMap = false;
	}
	if (useEntityMap == false && myActiveUnitsMap.find(aMessage.myGID) == myActiveUnitsMap.end())
	{
		return;
	}

	if (aMessage.myShouldActivate == true)
	{
		if (useEntityMap == true)
		{
			myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->AddToScene();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->AddToScene();
			}
		}
		else
		{
			myActiveUnitsMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->Wake();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveUnitsMap[aMessage.myGID]->AddToScene();
			}
		}
	}
	else
	{
		if (useEntityMap == true)
		{
			if (myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>() != nullptr)
			{
				myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->RemoveFromScene();
			}
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->RemoveFromScene();
			}
		}
		else
		{
			myActiveUnitsMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->Sleep();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveUnitsMap[aMessage.myGID]->RemoveFromScene();
			}
		}
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in&)
{
	if (aMessage.myGID == myPlayer->GetGID())
	{
		if (static_cast<eEntityState>(aMessage.myEntityState) == eEntityState::DIE)
		{
			myPlayer->SetState(static_cast<eEntityState>(aMessage.myEntityState));
		}
		else if (myPlayer->GetState() == eEntityState::DIE)
		{
			myPlayer->SetState(static_cast<eEntityState>(aMessage.myEntityState));
		}
		return;
	}

	if (myActiveUnitsMap.find(aMessage.myGID) == myActiveUnitsMap.end())
	{
		return;
	}
	myActiveUnitsMap[aMessage.myGID]->SetState(static_cast<eEntityState>(aMessage.myEntityState));
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in&)
{
	Entity* requestedBullet = ClientProjectileManager::GetInstance()->RequestBullet(aMessage.myGID);
	requestedBullet->AddToScene();
	ClientProjectileManager::GetInstance()->ActivateBullet(requestedBullet);

	const CU::GrowingArray<Entity*>& units = SharedUnitManager::GetInstance()->GetUnits();

	for (int i = 0; i < units.Size(); ++i)
	{
		if (units[i]->GetGID() == aMessage.myEnemyGID)
		{
			units[i]->GetComponent<AnimationComponent>()->PlayMuzzleFlash();
		}
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageShootGrenade&, const sockaddr_in&)
{
	Entity* bullet = ClientProjectileManager::GetInstance()->RequestGrenade();
	CU::Matrix44<float> playerOrientation = myPlayer->GetOrientation();
	bullet->AddToScene();
	bullet->GetComponent<PhysicsComponent>()->AddToScene();
	bullet->Reset();
	CU::Vector3<float> pos = playerOrientation.GetPos();
	pos.y += 1.5f;
	//bullet->GetComponent<PhysicsComponent>()->TeleportToPosition(pos);
	//bullet->GetComponent<GrenadeComponent>()->Activate(aMessage.mySenderID);
	//bullet->GetComponent<PhysicsComponent>()->AddForce(playerOrientation.GetForward(), float(aMessage.myForceStrength));
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageExplosion& aMessage, const sockaddr_in&)
{
	ClientProjectileManager::GetInstance()->RequestExplosion(aMessage.myPosition, aMessage.myGID);
	ClientProjectileManager::GetInstance()->KillGrenade(aMessage.myGID - 1);
	PostMaster::GetInstance()->SendMessage(EmitterMessage("Explosion", aMessage.myPosition, true));
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageRayCastRequest& aMessage, const sockaddr_in&)
{
	if (myPlayer->GetGID() != aMessage.myGID)
	{
		Entity* otherPlayer = nullptr;

		for (int i = 0; i < myPlayers.Size(); i++)
		{
			if (myPlayers[i]->GetGID() == aMessage.myGID)
			{
				otherPlayer = myPlayers[i];
				break;
			}
		}

		switch (eNetRayCastType(aMessage.myRayCastType))
		{
		case eNetRayCastType::CLIENT_SHOOT_PISTOL:
			Prism::PhysicsInterface::GetInstance()->RayCast(aMessage.myPosition, aMessage.myDirection, 500.f
				, myOtherClientRaycastHandlerPistol, otherPlayer->GetComponent<PhysicsComponent>());
			break;
		case eNetRayCastType::CLIENT_SHOOT_SHOTGUN:
			Prism::PhysicsInterface::GetInstance()->RayCast(aMessage.myPosition, aMessage.myDirection, 500.f
				, myOtherClientRaycastHandlerShotgun, otherPlayer->GetComponent<PhysicsComponent>());
			break;
		}

		otherPlayer->GetComponent<AnimationComponent>()->PlayMuzzleFlash();
	}
}

void ClientLevel::AddLight(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
}

void ClientLevel::AddLight(Prism::SpotLight* aLight)
{
	mySpotLights.Add(aLight);
	myScene->AddLight(aLight);
}

void ClientLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
{
	Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	switch (first.GetType())
	{
	case eEntityType::TRIGGER:
		HandleTrigger(first, second, aHasEntered);
		break;
	case eEntityType::EXPLOSION:
		if (aHasEntered == true)
		{
			HandleExplosion(first, second);
		}
		break;
	}
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

void ClientLevel::AddWorldText(const std::string& aText, const CU::Vector3<float>& aPosition, float aRotationAroundY, const CU::Vector4<float>& aColor)
{
	WorldText toAdd;
	toAdd.myProxy = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true, false);
	toAdd.myText = aText;
	toAdd.myProxy->SetOffset(aPosition);
	toAdd.myProxy->SetColor(aColor);
	toAdd.myProxy->Rotate3dText(aRotationAroundY);

	myWorldTexts.Add(toAdd);
}

void ClientLevel::HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered)
{
	if (aSecondEntity.GetType() == eEntityType::PLAYER)
	{
		if (aHasEntered == true)
		{
			TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();
			if (firstTrigger->GetTriggerType() == eTriggerType::UPGRADE)
			{
				myTextManager->AddNotification("upgrade");
				if (aSecondEntity.GetType() == eEntityType::PLAYER)
				{
					aSecondEntity.SendNote<UpgradeNote>(aFirstEntity.GetComponent<UpgradeComponent>()->GetData());
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInFirstLayer", 0);
				}
			}
			else if (firstTrigger->GetTriggerType() == eTriggerType::HEALTH_PACK)
			{
				myTextManager->AddNotification("healthpack");
				ClientNetworkManager::GetInstance()->AddMessage<NetMessageHealthPack>(NetMessageHealthPack(firstTrigger->GetValue()));
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInSecondLayer", 0);
			}
			aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity, aHasEntered));
		}
		aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity, aHasEntered));
	}
}

void ClientLevel::CreatePlayers()
{
	myPlayer = EntityFactory::GetInstance()->CreateEntity(ClientNetworkManager::GetInstance()->GetGID(), eEntityType::UNIT, "localplayer", myScene, true, CU::Vector3<float>(0.f, 0.f, 0.f));

	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	for each (const OtherClients& client in ClientNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		newPlayer->AddToScene();
		newPlayer->Reset();
		myPlayers.Add(newPlayer);
		myActiveUnitsMap[newPlayer->GetGID()] = newPlayer;
	}
}

void ClientLevel::HandleOtherClientRayCastPistol(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			aComponent->AddForce(aDirection, myForceStrengthPistol);
		}

		CU::Vector3<float> toSend = CU::Reflect<float>(aDirection, aHitNormal);
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Shotgun", aHitPosition, toSend));
	}
}

void ClientLevel::HandleOtherClientRayCastShotgun(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			aComponent->AddForce(aDirection, myForceStrengthShotgun);
		}
		CU::Vector3<float> toSend = CU::Reflect(aDirection, aHitNormal);
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Shotgun", aHitPosition, toSend));
	}
}