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
#include <LevelLoadedMessage.h>
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
#include <OnClickMessage.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <Renderer.h>
#include <CubeMapGenerator.h>
#include <PointLight.h>
#include <SpotLight.h>
#include <GrenadeComponent.h>
#include <GUIManager.h>

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
#include <LevelCompleteMessage.h>
#include <SoundComponent.h>
#include <VisualExplosionComponent.h>
#include <WidgetContainer.h>

ClientLevel::ClientLevel(GUI::Cursor* aCursor, eStateStatus& aStateStatus, int aLevelID)
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
	, mySpotLights(64)
	, mySpotLightsTextureProjection(64)
	, myInitDone(false)
	, myForceStrengthPistol(0.f)
	, myForceStrengthShotgun(0.f)
	, myWorldTexts(64)
	, myEscapeMenuActive(false)
	, myEscapeMenu(nullptr)
	, mySFXText(nullptr)
	, myMusicText(nullptr)
	, myVoiceText(nullptr)
	, myVisualExplosion(nullptr)
	, mySfxVolume(0)
	, myMusicVolume(0)
	, myVoiceVolume(0)
	, myStateStatus(aStateStatus)
	, myLevelID(aLevelID)
	, myHasStartedFirstLayer(false)
	, myHasStartedSecondLayer(false)
	, myHasStoppedFirstLayer(true)
	, myHasStoppedSecondLayer(true)
{
	Prism::PhysicsInterface::Create(std::bind(&ClientLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), false);

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENEMY_SHOOTING, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SHOOT_GRENADE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::EXPLOSION, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::RAY_CAST_REQUEST, this);

	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);

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

	myEscapeMenu = new GUI::GUIManager(aCursor, "Data/Resource/GUI/GUI_options_menu_ingame.xml", myScene->GetCamera(), -1);

	mySFXText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	myMusicText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	myVoiceText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));

	CU::Vector2<float> textPos(860.f, 660.f);

	mySFXText->SetPosition(textPos);
	textPos.y -= 60.f;
	myMusicText->SetPosition(textPos);
	textPos.y -= 60.f;
	myVoiceText->SetPosition(textPos);

	mySfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
	myMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
	myVoiceVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
	//while (myVoiceText->IsLoaded() == false || mySFXText->IsLoaded() == false || myMusicText->IsLoaded() == false)
	//{
	//}
	GC::PlayerAlive = true;
	

}

ClientLevel::~ClientLevel()
{
	PostMaster::GetInstance()->SendMessage<LevelCompleteMessage>(LevelCompleteMessage(myLevelID));
	for (int i = 0; i < myWorldTexts.Size(); ++i)
	{
		SAFE_DELETE(myWorldTexts[i].myProxy);
	}
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

	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	mySpotLights.DeleteAll();
	mySpotLightsTextureProjection.DeleteAll();
	myPlayers.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);
	SAFE_DELETE(myFullscreenRenderer);
	SAFE_DELETE(myTextManager);
	SAFE_DELETE(myEscapeMenu);

	SAFE_DELETE(mySFXText);
	SAFE_DELETE(myMusicText);
	SAFE_DELETE(myVoiceText);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);

}

void ClientLevel::Init(const std::string& aWeaponSettingsPath)
{
	CreatePlayers();
	myVisualExplosion = EntityFactory::CreateEntity(32145679, eEntityType::VISUAL_EXPLOSION, myScene, true, CU::Vector3<float>());
	myVisualExplosion->AddToScene();

	Prism::ModelLoader::GetInstance()->Pause();
	myDeferredRenderer = new Prism::DeferredRenderer();

	myFullscreenRenderer = new Prism::Renderer();


	Prism::ModelLoader::GetInstance()->UnPause();
	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);
	ClientProjectileManager::GetInstance()->CreateBullets(myScene);
	ClientProjectileManager::GetInstance()->CreateGrenades(myScene);
	ClientProjectileManager::GetInstance()->CreateExplosions();

	myTextManager = new TextEventManager(myPlayer->GetComponent<InputComponent>()->GetCamera());
	myEmitterManager->Initiate(myPlayer->GetComponent<InputComponent>()->GetCamera());

	myPlayer->GetComponent<ShootingComponent>()->ReadXMLSettings(aWeaponSettingsPath);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->Init();

	myForceStrengthPistol = myPlayer->GetComponent<ShootingComponent>()->GetWeaponForceStrength(eWeaponType::PISTOL);
	myForceStrengthShotgun = myPlayer->GetComponent<ShootingComponent>()->GetWeaponForceStrength(eWeaponType::SHOTGUN);
	Prism::PhysicsInterface::GetInstance()->SetClientSide(true);

	Prism::ModelLoader::GetInstance()->Pause();
	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	Prism::ModelLoader::GetInstance()->UnPause();

}

void ClientLevel::SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint)
{
	myMinPoint = aMinPoint;
	myMaxPoint = aMaxPoint;
}

void ClientLevel::SetName(const std::string& aName)
{
	myName = aName;
	myDeferredRenderer->SetCubeMap(aName);
}

void ClientLevel::Update(const float aDeltaTime, bool aLoadingScreen)
{
	if (myInitDone == false && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MainMenu", 0);
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_AllElevators", 0);
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ElevatorPling", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayAll", 0);

		myInitDone = true;

		SET_RUNTIME(false);
		myDeferredRenderer->LoadSHData(myMinPoint, myMaxPoint, myName);
		RESET_RUNTIME;

		//for (int i = 0; i < myActiveEntities.Size(); ++i)
		//{
		//	if (myActiveEntities[i]->GetSubType() == "SM_elevator_a_open")
		//	{
		//		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_3DElevatorToLevel0", 0);
		//	}
		//}

		PostMaster::GetInstance()->SendMessage<LevelLoadedMessage>(LevelLoadedMessage(myLevelID));
	}

	SharedLevel::Update(aDeltaTime, aLoadingScreen);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->UpdateCoOpPositions(myPlayers);
	myPlayer->Update(aDeltaTime);

	if (myInitDone == false || aLoadingScreen == true)
	{
		return;
	}

	ClientProjectileManager::GetInstance()->Update(aDeltaTime);
	ClientUnitManager::GetInstance()->Update(aDeltaTime);


	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_L))
	{
		myDeferredRenderer->GenerateSHData(myScene, myMinPoint, myMaxPoint, myName);
	}

	if (myWorldTexts.Size() > 0)
	{
		//if (Prism::ModelLoader::GetInstance()->IsLoading() == false && myWorldTexts[0].myProxy->IsLoaded() == true)
		{
			for (int i = 0; i < myWorldTexts.Size(); ++i)
			{
				myWorldTexts[i].myProxy->SetText(myWorldTexts[i].myText);
			}
		}
	}

	myVisualExplosion->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());
	myTextManager->Update(aDeltaTime);

	int enemiesInPlayerRange = ClientUnitManager::GetInstance()->GetUnitsInPlayerRange(myPlayer->GetOrientation().GetPos());
	if (enemiesInPlayerRange > 0 && enemiesInPlayerRange <= 4 && myHasStartedFirstLayer == false)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInFirstLayer", 0);
		myHasStartedFirstLayer = true;
		myHasStoppedFirstLayer = false;
	}
	if (enemiesInPlayerRange > 4 && myHasStartedSecondLayer == false)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInSecondLayer", 0);
		myHasStartedSecondLayer = true;
		myHasStoppedSecondLayer = false;
	}
	if (enemiesInPlayerRange < 1 && myHasStoppedFirstLayer == false)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutFirstLayer", 0);
		myHasStoppedFirstLayer = true;
		myHasStartedFirstLayer = false;

		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutSecondLayer", 0);
		myHasStoppedSecondLayer = true;
		myHasStartedSecondLayer = false;
	}


	unsigned long long ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(int(ms));
	DEBUG_PRINT(kbs);

	if (myEscapeMenuActive == true)
	{
		myEscapeMenu->Update(aDeltaTime);

	}
	Prism::PhysicsInterface::GetInstance()->EndFrame();




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
				myScene->GetRoomManager()->GetPreviousPlayerRoom()->GetEmitter()->SetShouldRender(false);
			}
		}

		myEmitterManager->RenderEmitters();

		if (GC::ShouldRenderGUI == true)
		{
			myPlayer->GetComponent<FirstPersonRenderComponent>()->Render(myDeferredRenderer->GetArmDepthStencilTexture(), myLevelID == 0);

			myTextManager->Render();

			for (int i = 0; i < myWorldTexts.Size(); ++i)
			{
				if (CU::Length(myWorldTexts[i].myProxy->Get3DPosition() - myPlayer->GetOrientation().GetPos()) <= 10.f)
				{
					myWorldTexts[i].myProxy->Render(myScene->GetCamera());
				}
			}
		}



		if (myEscapeMenuActive == true)
		{
			myEscapeMenu->Render();
			mySFXText->Render();
			myMusicText->Render();
			myVoiceText->Render();
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
			myTextManager->AddRespawnText("", false);
			myPlayer->GetComponent<PhysicsComponent>()->Wake();
			GC::PlayerAlive = true;
		}
		else
		{
			myTextManager->AddRespawnText("System shutdown\nYour ally can revive", true, { 1.f, 0.f, 0.f, 1.f });
			myPlayer->GetComponent<PhysicsComponent>()->Sleep();
			GC::PlayerAlive = false;
		}
		return;
	}
	else
	{
		for (int i = 0; i < myPlayers.Size(); ++i)
		{
			if (aMessage.myGID == myPlayers[i]->GetGID())
			{
				if (aMessage.myShouldActivate == true)
				{
					myTextManager->AddRespawnText("", false);
				}
				else
				{
					myTextManager->AddRespawnText("Your ally is down\nRevive him/her", true, { 1.f, 0.f, 0.f, 1.f });
				}
				break;
			}
		}
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
			if (myActiveEntitiesMap[aMessage.myGID]->GetComponent<SoundComponent>() != nullptr)
			{
				if (myActiveEntitiesMap[aMessage.myGID]->GetSubType() == "sm_double_doors_a_400_300_laser")
				{
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Door"
						, myActiveEntitiesMap[aMessage.myGID]->GetComponent<SoundComponent>()->GetAudioSFXID());
				}
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
				if (myActiveEntitiesMap[aMessage.myGID]->GetComponent<SoundComponent>() != nullptr)
				{
					if (myActiveEntitiesMap[aMessage.myGID]->GetSubType() == "sm_double_doors_a_400_300_laser")
					{
						Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Door"
							, myActiveEntitiesMap[aMessage.myGID]->GetComponent<SoundComponent>()->GetAudioSFXID());
						Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_DoorShutDown"
							, myActiveEntitiesMap[aMessage.myGID]->GetComponent<SoundComponent>()->GetAudioSFXID());
					}
				}
			}
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->RemoveFromScene();
				PostMaster::GetInstance()->SendMessage(EmitterMessage(myActiveEntitiesMap[aMessage.myGID]->GetEmitter(), true));
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
		else if (myPlayer->GetState() == eEntityState::DIE && static_cast<eEntityState>(aMessage.myEntityState) == eEntityState::IDLE)
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
		if (units[i]->GetGID() == unsigned int(aMessage.myEnemyGID))
		{
			units[i]->GetComponent<AnimationComponent>()->PlayMuzzleFlash();
			if (units[i]->GetComponent<SoundComponent>() != nullptr)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EnemyShoot", units[i]->GetComponent<SoundComponent>()->GetAudioSFXID());
			}
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
	myVisualExplosion->GetComponent<VisualExplosionComponent>()->SetPosition(aMessage.myPosition);
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
			if (otherPlayer->GetComponent<SoundComponent>() != nullptr)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_OtherPlayerPistol"
					, otherPlayer->GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			break;
		case eNetRayCastType::CLIENT_SHOOT_SHOTGUN:
			Prism::PhysicsInterface::GetInstance()->RayCast(aMessage.myPosition, aMessage.myDirection, 500.f
				, myOtherClientRaycastHandlerShotgun, otherPlayer->GetComponent<PhysicsComponent>());
			if (otherPlayer->GetComponent<SoundComponent>() != nullptr)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_OtherPlayerShotgun"
					, otherPlayer->GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			break;
		}

		otherPlayer->GetComponent<AnimationComponent>()->PlayMuzzleFlash();
	}
}

void ClientLevel::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::RESUME_GAME:
		ToggleEscapeMenu();
		break;
	case eOnClickEvent::HELP:
		break;
	case eOnClickEvent::GAME_QUIT:
		PostMaster::GetInstance()->SendMessage<LevelCompleteMessage>(LevelCompleteMessage(myLevelID));
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
		myStateStatus = eStateStatus::ePopMainState;
		break;
	case eOnClickEvent::INCREASE_VOLUME:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVolume", 0);
		mySfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
		mySFXText->SetText("SFX: " + std::to_string(mySfxVolume));
		break;
	case eOnClickEvent::LOWER_VOLUME:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVolume", 0);
		mySfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
		mySFXText->SetText("SFX: " + std::to_string(mySfxVolume));
		break;
	case eOnClickEvent::INCREASE_MUSIC:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseMusic", 0);
		myMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
		myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
		break;
	case eOnClickEvent::LOWER_MUSIC:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerMusic", 0);
		myMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
		myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
		break;
	case eOnClickEvent::INCREASE_VOICE:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVoice", 0);
		myVoiceVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
		myVoiceText->SetText("Voice: " + std::to_string(myVoiceVolume));
		break;
	case eOnClickEvent::LOWER_VOICE:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVoice", 0);
		myVoiceVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
		myVoiceText->SetText("Voice: " + std::to_string(myVoiceVolume));
		break;
	default:
		DL_ASSERT("Not implemented this onclickevent");
		break;
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

void ClientLevel::AddLight(Prism::SpotLightTextureProjection* aLight)
{
	mySpotLightsTextureProjection.Add(aLight);
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

void ClientLevel::ToggleEscapeMenu()
{
	mySFXText->SetText("SFX: " + std::to_string(mySfxVolume));
	myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
	myVoiceText->SetText("Voice: " + std::to_string(myVoiceVolume));

	myEscapeMenuActive = !myEscapeMenuActive;
	myEscapeMenu->SetMouseShouldRender(myEscapeMenuActive);

	myPlayer->GetComponent<InputComponent>()->SetIsInOptionsMenu(myEscapeMenuActive);
	myPlayer->GetComponent<ShootingComponent>()->SetIsInOptionsMenu(myEscapeMenuActive);
}

void ClientLevel::OnResize(float aWidth, float aHeight)
{
	myFullscreenRenderer->OnResize(aWidth, aHeight);
	myDeferredRenderer->OnResize(aWidth, aHeight);
	myEscapeMenu->OnResize(int(aWidth), int(aHeight));
	myPlayer->GetComponent<FirstPersonRenderComponent>()->OnResize({ aWidth, aHeight });

	CU::Vector2<float> floatScreenPos(float(aWidth), float(aHeight));
	GUI::WidgetContainer* widgetCont = reinterpret_cast<GUI::WidgetContainer*>(myEscapeMenu->GetWidgetContainer()->At(0));
	CU::Vector2<float> firstRow = widgetCont->At(3)->GetPosition();
	CU::Vector2<float> secondRow = widgetCont->At(1)->GetPosition();
	CU::Vector2<float> thirdRow = widgetCont->At(5)->GetPosition();
	float moveAmount = 320.f;
	float ratio = static_cast<float>(aWidth) / static_cast<float>(aHeight);

	float epsilon = 0.1f;
	if (ratio + epsilon >= 16.f / 9.f)
	{
		moveAmount = 320.f;
	}
	else if (ratio + epsilon >= 16.f / 10.f)
	{
		moveAmount = 320.f;
	}
	else
	{
		moveAmount = 220.f;
	}
	myMusicText->SetPosition({ firstRow.x - moveAmount, firstRow.y - (widgetCont->At(1)->GetSize().y * 0.25f) });
	mySFXText->SetPosition({ secondRow.x - moveAmount, secondRow.y - (widgetCont->At(3)->GetSize().y * 0.25f) });
	myVoiceText->SetPosition({ thirdRow.x - moveAmount, thirdRow.y - (widgetCont->At(5)->GetSize().y * 0.2f) });
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
				//myTextManager->AddNotification("upgrade");
				if (aSecondEntity.GetType() == eEntityType::PLAYER)
				{
					aSecondEntity.SendNote<UpgradeNote>(aFirstEntity.GetComponent<UpgradeComponent>()->GetData());
					PostMaster::GetInstance()->SendMessage(EmitterMessage(firstTrigger->GetEntity().GetEmitter(), true));
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Positive", 0);
				}
			}

			aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity, aHasEntered));
		}
		aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity, aHasEntered));
	}
}

void ClientLevel::CreatePlayers()
{
	myPlayer = EntityFactory::GetInstance()->CreateEntity(ClientNetworkManager::GetInstance()->GetGID(), eEntityType::UNIT, "localplayer", myScene, true
		, myPlayerStartPositions[ClientNetworkManager::GetInstance()->GetGID()]);

	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	for each (const OtherClients& client in ClientNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", myScene, true, myPlayerStartPositions[client.myID]);
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
		if (aComponent->GetEntity().GetIsEnemy() == true)
		{
			PostMaster::GetInstance()->SendMessage(EmitterMessage("OnHit", aHitPosition));
		}
		else
		{
			CU::Vector3<float> toMove = aHitPosition;
			toMove += (aHitNormal * 0.1f);
			PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit", toMove));
			PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit_2", toMove));
		}
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

		if (aComponent->GetEntity().GetIsEnemy() == true)
		{
			PostMaster::GetInstance()->SendMessage(EmitterMessage("OnHit", aHitPosition));
		}
		else
		{
			CU::Vector3<float> toMove = aHitPosition;
			toMove += (aHitNormal * 0.1f);
			PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit", toMove));
			PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit_2", toMove));
		}
	}
}