#include "stdafx.h"

#include <Animation.h>
#include <AnimationSystem.h>
#include <Camera.h>
#include <Engine.h>
#include "Entity.h"
#include "FirstPersonRenderComponent.h"
#include <GUIManager3D.h>
#include "HealthComponent.h"
#include <Instance.h>
#include "InputComponent.h"
#include <ModelLoader.h>
#include <NetMessageDisplayMarker.h>
#include <NetMessageHealth.h>
#include <Scene.h>
#include "ShootingComponent.h"
#include <SpriteProxy.h>
#include <NetworkMessageTypes.h>
#include <SharedNetworkManager.h>
#include <Engine.h>
#include <NetMessageOnHit.h>

FirstPersonRenderComponent::FirstPersonRenderComponent(Entity& aEntity, Prism::Scene* aScene)
	: Component(aEntity)
	, myInputComponentEyeOrientation(myEntity.GetComponent<InputComponent>()->GetEyeOrientation())
	, myCurrentState(ePlayerState::PISTOL_IDLE)
	, myIntentions(8)
	, myFirstTimeActivateAnimation(false)
	, myCoOpPositions(8)
	, myDisplayDamageIndicatorTimer(0)
	, myMaxHealth(10)
	, myCurrentHealth(myMaxHealth)
	, myHasDied(false)
	, myScene(aScene)
	, myRenderMarker(false)
{
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);
	CU::Vector2<float> damageSize(Prism::Engine::GetInstance()->GetWindowSize().x, Prism::Engine::GetInstance()->GetWindowSize().y);
	myDamageIndicator = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_damage_indicator.dds", damageSize, damageSize * 0.5f);
	myCoOpSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_coopmarker.dds", size, size * 0.5f);
	myMarker = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_coopmarker.dds", size, size * 0.5f);

	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/Pistol/SK_arm_pistol_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myModel = new Prism::Instance(*model, myInputComponentEyeOrientation);
	aScene->AddInstance(myModel, eObjectRoomType::ALWAYS_RENDER);

	AddAnimation(ePlayerState::PISTOL_IDLE, "Data/Resource/Model/First_person/Pistol/SK_arm_pistol_idle.fbx", true, true);
	AddAnimation(ePlayerState::PISTOL_HOLSTER, "Data/Resource/Model/First_person/Pistol/SK_arm_pistol_holster.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_DRAW, "Data/Resource/Model/First_person/Pistol/SK_arm_pistol_draw.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_RELOAD, "Data/Resource/Model/First_person/Pistol/SK_arm_pistol_reload.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_FIRE, "Data/Resource/Model/First_person/Pistol/SK_arm_pistol_fire.fbx", false, true);

	AddAnimation(ePlayerState::SHOTGUN_IDLE, "Data/Resource/Model/First_person/Shotgun/SK_arm_shotgun_idle.fbx", true, true);
	AddAnimation(ePlayerState::SHOTGUN_HOLSTER, "Data/Resource/Model/First_person/Shotgun/SK_arm_shotgun_holster.fbx", false, true);
	AddAnimation(ePlayerState::SHOTGUN_DRAW, "Data/Resource/Model/First_person/Shotgun/SK_arm_shotgun_draw.fbx", false, true);
	AddAnimation(ePlayerState::SHOTGUN_RELOAD, "Data/Resource/Model/First_person/Shotgun/SK_arm_shotgun_reload.fbx", false, true);
	AddAnimation(ePlayerState::SHOTGUN_FIRE, "Data/Resource/Model/First_person/Shotgun/SK_arm_shotgun_fire.fbx", false, true);

	AddAnimation(ePlayerState::GRENADE_LAUNCHER_IDLE, "Data/Resource/Model/First_person/GrenadeLauncher/SK_arm_grenade_launcher_idle.fbx", true, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_HOLSTER, "Data/Resource/Model/First_person/GrenadeLauncher/SK_arm_grenade_launcher_holster.fbx", false, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_DRAW, "Data/Resource/Model/First_person/GrenadeLauncher/SK_arm_grenade_launcher_draw.fbx", false, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_RELOAD, "Data/Resource/Model/First_person/GrenadeLauncher/SK_arm_grenade_launcher_reload.fbx", false, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_FIRE, "Data/Resource/Model/First_person/GrenadeLauncher/SK_arm_grenade_launcher_fire.fbx", false, true);

	myModel->Update(1.f / 30.f);

	for (int i = 0; i < static_cast<int>(ePlayerState::_COUNT); ++i)
	{
		Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myAnimations[i].myData.myFile, "ui_jnt2", myAnimations[i].myUIBone);
		Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myAnimations[i].myData.myFile, "health_jnt2", myAnimations[i].myHealthBone);
	}

	ShootingComponent* shooting = myEntity.GetComponent<ShootingComponent>();
	my3DGUIManager = new GUI::GUIManager3D(myModel, aScene
		, shooting->GetWeapon(eWeaponType::PISTOL)->GetClipSize(), shooting->GetWeapon(eWeaponType::PISTOL)->GetAmmoInClip()
		, shooting->GetWeapon(eWeaponType::SHOTGUN)->GetClipSize(), shooting->GetWeapon(eWeaponType::SHOTGUN)->GetAmmoInClip()
		, shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetClipSize(), shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetAmmoInClip());

	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_HIT, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::HEALTH, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::DISPLAY_MARKER, this);
	
	Prism::ModelProxy* pistolModel = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/Pistol/SK_pistol_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myPistolModel = new Prism::Instance(*pistolModel, myInputComponentEyeOrientation);
	AddWeaponAnimation(ePlayerState::PISTOL_IDLE, "Data/Resource/Model/First_person/Pistol/SK_pistol_idle.fbx", true, true);
	AddWeaponAnimation(ePlayerState::PISTOL_HOLSTER, "Data/Resource/Model/First_person/Pistol/SK_pistol_holster.fbx", false, true);
	AddWeaponAnimation(ePlayerState::PISTOL_DRAW, "Data/Resource/Model/First_person/Pistol/SK_pistol_draw.fbx", false, true);
	AddWeaponAnimation(ePlayerState::PISTOL_RELOAD, "Data/Resource/Model/First_person/Pistol/SK_pistol_reload.fbx", false, true);
	AddWeaponAnimation(ePlayerState::PISTOL_FIRE, "Data/Resource/Model/First_person/Pistol/SK_pistol_fire.fbx", false, true);
	myPistolModel->Update(1.f / 30.f);

	Prism::ModelProxy* shotgunModel = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/Shotgun/SK_shotgun_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myShotgunModel = new Prism::Instance(*shotgunModel, myInputComponentEyeOrientation);
	AddWeaponAnimation(ePlayerState::SHOTGUN_IDLE, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_idle.fbx", true, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_HOLSTER, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_holster.fbx", false, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_DRAW, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_draw.fbx", false, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_RELOAD, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_reload.fbx", false, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_FIRE, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_fire.fbx", false, true);
	myShotgunModel->Update(1.f / 30.f);

	Prism::ModelProxy* grenadeModel = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myGrenadeLauncherModel = new Prism::Instance(*grenadeModel, myInputComponentEyeOrientation);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_IDLE, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_idle.fbx", true, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_HOLSTER, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_holster.fbx", false, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_DRAW, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_draw.fbx", false, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_RELOAD, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_reload.fbx", false, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_FIRE, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_fire.fbx", false, true);
	myGrenadeLauncherModel->Update(1.f / 30.f);

	myCurrentWeaponModel = myPistolModel;
	aScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);
}

FirstPersonRenderComponent::~FirstPersonRenderComponent()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_HIT, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::HEALTH, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::DISPLAY_MARKER, this);
	SAFE_DELETE(myCrosshair);
	SAFE_DELETE(myDamageIndicator);
	SAFE_DELETE(my3DGUIManager);
	SAFE_DELETE(myModel);
	SAFE_DELETE(myCoOpSprite);
	SAFE_DELETE(myCurrentWeaponModel);
	SAFE_DELETE(myPistolModel);
	SAFE_DELETE(myShotgunModel);
	SAFE_DELETE(myGrenadeLauncherModel);
	SAFE_DELETE(myMarker);
	myCoOpPositions.RemoveAll();
}


void FirstPersonRenderComponent::Update(float aDelta)
{
	if (myEntity.GetState() == eEntityState::DIE && myHasDied == false)
	{
		myHasDied = true;
		myEntity.GetScene()->RemoveInstance(myModel);
	}
	if (myEntity.GetState() != eEntityState::DIE && myHasDied == true)
	{
		myHasDied = false;
		myEntity.GetScene()->AddInstance(myModel, eObjectRoomType::DYNAMIC);
	}

	UpdateJoints();
	if (myDisplayDamageIndicatorTimer >= 0.f)
	{
		myDisplayDamageIndicatorTimer -= aDelta;
	}

	my3DGUIManager->Update(myUIJoint, myHealthJoint, myCurrentHealth
		, myMaxHealth, aDelta);



	if (myCurrentState == ePlayerState::PISTOL_IDLE
		|| myCurrentState == ePlayerState::SHOTGUN_IDLE
		|| myCurrentState == ePlayerState::GRENADE_LAUNCHER_IDLE)
	{
		if (myIntentions.Size() > 0)
		{
			myCurrentState = myIntentions[0];
			myIntentions.RemoveNonCyclicAtIndex(0);

			PlayAnimation(myCurrentState);
		}
	}



	Prism::AnimationData& data = myAnimations[int(myCurrentState)].myData;
	if (myModel->IsAnimationDone() == true && data.myShouldLoop == false)
	{
		bool shouldGetNextIntention = false;
		switch (myEntity.GetComponent<ShootingComponent>()->GetCurrentWeapon()->GetWeaponType())
		{
		case eWeaponType::PISTOL:
			if (myCurrentState == ePlayerState::SHOTGUN_HOLSTER
				|| myCurrentState == ePlayerState::GRENADE_LAUNCHER_HOLSTER)
			{
				shouldGetNextIntention = true;
			}
			else
			{
				myCurrentState = ePlayerState::PISTOL_IDLE;
				PlayAnimation(myCurrentState);
			}
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			if (myCurrentState == ePlayerState::PISTOL_HOLSTER
				|| myCurrentState == ePlayerState::SHOTGUN_HOLSTER)
			{
				shouldGetNextIntention = true;
			}
			else
			{
				myCurrentState = ePlayerState::GRENADE_LAUNCHER_IDLE;
				PlayAnimation(myCurrentState);
			}
			break;
		case eWeaponType::SHOTGUN:
			if (myCurrentState == ePlayerState::PISTOL_HOLSTER 
				|| myCurrentState == ePlayerState::GRENADE_LAUNCHER_HOLSTER)
			{
				shouldGetNextIntention = true;
			}
			else
			{
				myCurrentState = ePlayerState::SHOTGUN_IDLE;
				PlayAnimation(myCurrentState);
			}
			break;
		}
		if (shouldGetNextIntention == true)
		{
			if (myIntentions.Size() > 0)
			{
				myCurrentState = myIntentions[0];
				myIntentions.RemoveNonCyclicAtIndex(0);

				PlayAnimation(myCurrentState);
			}
		}
	}
	if (myModel->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myModel->Update(aDelta);

	}
	data.myElapsedTime += aDelta;

	Prism::AnimationData& weaponData = myWeaponAnimations[int(myCurrentState)];
	if (myCurrentWeaponModel->IsAnimationDone() == false || weaponData.myShouldLoop == true)
	{
		myCurrentWeaponModel->Update(aDelta);

	}
	weaponData.myElapsedTime += aDelta;
}

void FirstPersonRenderComponent::UpdateCoOpPositions(const CU::GrowingArray<Entity*>& somePlayers)
{
	myCoOpPositions.RemoveAll();
	for (int i = 0; i < somePlayers.Size(); ++i)
	{
		myCoOpPositions.Add(somePlayers[i]->GetOrientation().GetPos());
	}
}

void FirstPersonRenderComponent::Render()
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);
	if (myDisplayDamageIndicatorTimer > 0.f)
	{
		myDamageIndicator->Render(windowSize * 0.5f);
	}
	my3DGUIManager->Render();

	if (myRenderMarker == true)
	{
		CU::Matrix44<float> renderPos;
		CU::Vector3<float> tempPos(myMarkerPosition);
		tempPos.y += 2.f;
		float toBuddy = CU::Dot(tempPos - myInputComponentEyeOrientation.GetPos(), myInputComponentEyeOrientation.GetForward());
		if (toBuddy >= 0.f)
		{
			renderPos.SetPos(tempPos);
			renderPos = renderPos * CU::InverseSimple(myInputComponentEyeOrientation);
			renderPos = renderPos * myEntity.GetComponent<InputComponent>()->GetCamera()->GetProjection();

			CU::Vector3<float> newRenderPos = renderPos.GetPos();
			newRenderPos /= renderPos.GetPos4().w;

			newRenderPos += 1.f;
			newRenderPos *= 0.5f;
			newRenderPos.x *= windowSize.x;
			newRenderPos.y *= windowSize.y;
			newRenderPos.y -= windowSize.y;

			newRenderPos.x = fmaxf(0.f, fminf(newRenderPos.x, windowSize.x));
			newRenderPos.y += windowSize.y;
			newRenderPos.y = fmaxf(0.f, fminf(newRenderPos.y, windowSize.y));
			myMarker->Render({ newRenderPos.x, newRenderPos.y });
		}
	}

	for (int i = 0; i < myCoOpPositions.Size(); ++i)
	{
		CU::Matrix44<float> renderPos;
		CU::Vector3<float> tempPos(myCoOpPositions[i]);
		tempPos.y += 2.f;
		float toBuddy = CU::Dot(tempPos - myInputComponentEyeOrientation.GetPos(), myInputComponentEyeOrientation.GetForward());
		if (toBuddy < 0.f)
		{
			continue;
		}
		renderPos.SetPos(tempPos);
		renderPos = renderPos * CU::InverseSimple(myInputComponentEyeOrientation);
		renderPos = renderPos * myEntity.GetComponent<InputComponent>()->GetCamera()->GetProjection();

		CU::Vector3<float> newRenderPos = renderPos.GetPos();
		newRenderPos /= renderPos.GetPos4().w;

		newRenderPos += 1.f;
		newRenderPos *= 0.5f;
		newRenderPos.x *= windowSize.x;
		newRenderPos.y *= windowSize.y;
		newRenderPos.y -= windowSize.y;

		newRenderPos.x = fmaxf(0.f, fminf(newRenderPos.x, windowSize.x));
		newRenderPos.y += windowSize.y;
		newRenderPos.y = fmaxf(0.f, fminf(newRenderPos.y, windowSize.y));

		myCoOpSprite->Render({ newRenderPos.x, newRenderPos.y });
	}
}

bool FirstPersonRenderComponent::IsCurrentAnimationDone() const
{
	return myModel->IsAnimationDone();
}

void FirstPersonRenderComponent::RestartCurrentAnimation()
{
	myModel->ResetAnimationTime(0.f);
}

const CU::Matrix44<float>& FirstPersonRenderComponent::GetUIJointOrientation() const
{
	return myUIJoint;
}

void FirstPersonRenderComponent::AddAnimation(ePlayerState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	Prism::AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)].myData = newData;
}

void FirstPersonRenderComponent::AddWeaponAnimation(ePlayerState aState, const std::string& aAnimationPath, bool aLoopFlag, bool aResetTimeOnRestart)
{
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	Prism::AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myWeaponAnimations[int(aState)] = newData;
}

void FirstPersonRenderComponent::PlayAnimation(ePlayerState aAnimationState)
{
	myCurrentState = aAnimationState;
	if (myCurrentState == ePlayerState::PISTOL_DRAW)
	{
		myScene->RemoveInstance(myCurrentWeaponModel);
		myCurrentWeaponModel = myPistolModel;
		myScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);
	}
	else if (myCurrentState == ePlayerState::SHOTGUN_DRAW)
	{
		myScene->RemoveInstance(myCurrentWeaponModel);
		myCurrentWeaponModel = myShotgunModel;
		myScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);
	}
	else if (myCurrentState == ePlayerState::GRENADE_LAUNCHER_DRAW)
	{
		myScene->RemoveInstance(myCurrentWeaponModel);
		myCurrentWeaponModel = myGrenadeLauncherModel;
		myScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);
	}

	Prism::AnimationData& data = myAnimations[int(aAnimationState)].myData;
	myModel->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));

	if (data.myResetTimeOnRestart == true)
	{
		myModel->ResetAnimationTime(0.f);
	}
	else
	{
		myModel->ResetAnimationTime(data.myElapsedTime);
	}

	Prism::AnimationData& weaponData = myWeaponAnimations[int(aAnimationState)];
	myCurrentWeaponModel->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(weaponData.myFile.c_str()));

	if (weaponData.myResetTimeOnRestart == true)
	{
		myCurrentWeaponModel->ResetAnimationTime(0.f);
	}
	else
	{
		myCurrentWeaponModel->ResetAnimationTime(weaponData.myElapsedTime);
	}
}

void FirstPersonRenderComponent::AddIntention(ePlayerState aPlayerState, bool aClearIntentions)
{
	if (aClearIntentions == true)
	{
		myIntentions.RemoveAll();
		switch (myEntity.GetComponent<ShootingComponent>()->GetCurrentWeapon()->GetWeaponType())
		{
		case eWeaponType::PISTOL:
			myCurrentState = ePlayerState::PISTOL_IDLE;
			PlayAnimation(myCurrentState);
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			myCurrentState = ePlayerState::GRENADE_LAUNCHER_IDLE;
			PlayAnimation(myCurrentState);
			break;
		case eWeaponType::SHOTGUN:
			myCurrentState = ePlayerState::SHOTGUN_IDLE;
			PlayAnimation(myCurrentState);
			break;
		}
	}
	myIntentions.Add(aPlayerState);
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in&)
{
	if (aMessage.myGID == myEntity.GetGID())
	{
		myDisplayDamageIndicatorTimer = 0.5f;
	}
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessageHealth& aMessage, const sockaddr_in&)
{
	myMaxHealth = aMessage.myMaxHealth;
	myCurrentHealth = aMessage.myCurrentHealth;
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessageDisplayMarker& aMessage, const sockaddr_in&)
{
	myRenderMarker = aMessage.myDisplayMarker;
	myMarkerPosition = aMessage.myPosition;
}

void FirstPersonRenderComponent::UpdateJoints()
{
	PlayerAnimationData& currentAnimation = myAnimations[static_cast<int>(myCurrentState)];
	if (currentAnimation.IsValid() == true)
	{
		if (myFirstTimeActivateAnimation == false)
		{
			myFirstTimeActivateAnimation = true;
			//PlayAnimation(ePlayerState::PISTOL_IDLE);
			AddIntention(ePlayerState::PISTOL_FIRE, true);
		}

		myUIJoint = CU::InverseSimple(*currentAnimation.myUIBone.myBind) * (*currentAnimation.myUIBone.myJoint) * myInputComponentEyeOrientation;
		myHealthJoint = CU::InverseSimple(*currentAnimation.myHealthBone.myBind) * (*currentAnimation.myHealthBone.myJoint) * myInputComponentEyeOrientation;
	}
}