#include "stdafx.h"

#include <AudioInterface.h>
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
#include <ModelProxy.h>
#include <ModelLoader.h>
#include <NetMessageDisplayMarker.h>
#include <NetMessageDisplayRespawn.h>
#include <NetMessageHealth.h>
#include <NetMessagePressEText.h>
#include <Scene.h>
#include "ShootingComponent.h"
#include <SpriteProxy.h>
#include <NetworkMessageTypes.h>
#include <SharedNetworkManager.h>
#include <Engine.h>
#include <NetMessageOnHit.h>
#include "UpgradeNote.h"
#include <Texture.h>

FirstPersonRenderComponent::FirstPersonRenderComponent(Entity& aEntity, Prism::Scene* aScene)
	: Component(aEntity)
	, myInputComponentEyeOrientation(myEntity.GetComponent<InputComponent>()->GetEyeOrientation())
	, myCurrentState(ePlayerState::PISTOL_IDLE)
	, myIntentions(8)
	, myFirstTimeActivateAnimation(false)
	, myCoOpCircles(8)
	, myCoOpRespawns(8)
	, myDisplayDamageIndicatorTimer(0)
	, myDisplayDamageIndicatorTimerMax(0.7f)
	, myDisplayHealthIndicatorTimer(0)
	, myDisplayUpgradeIndicatorTimer(0)
	, myMaxHealth(1000)
	, myCurrentHealth(myMaxHealth)
	, myHasDied(false)
	, myScene(aScene)
	, myRenderMarker(false)
	, myPressETexts(16)
	, myPistolHasUpdated(false)
	, myShotgunHasUpdated(false)
	, myGrenadeLauncherHasUpdated(false)
	, myDisplayPickupTime(2.f)
	, myIsCoOp(false)
{
	CoOpCircle coopCircle;
	coopCircle.myPosition = { 0.f, 0.f, 0.f };
	coopCircle.myLifePercentage = 1.f;
	myCoOpCircles.Add(coopCircle);

	bool shouldUseSpecialFoV = true;
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);
	CU::Vector2<float> damageSize(Prism::Engine::GetInstance()->GetWindowSize().x, Prism::Engine::GetInstance()->GetWindowSize().y);
	myDamageIndicator = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_damage_indicator.dds", damageSize, damageSize * 0.5f);
	myPickupIndicator = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_pickup_indicator.dds", damageSize, damageSize * 0.5f);
	myLowLifeIndicator = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_low_life_indicator.dds", damageSize, damageSize * 0.5f);
	myCoOpSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_coopmarker.dds", size, size * 0.5f);
	myMarker = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_marker.dds", size, size * 0.5f);
	myIsDeadScreen = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_dead_indicator.dds", damageSize, damageSize * 0.5f);

	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/Pistol/SK_arm_pistol_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myModel = new Prism::Instance(*model, myInputComponentEyeOrientation, shouldUseSpecialFoV);
	//aScene->AddInstance(myModel, eObjectRoomType::ALWAYS_RENDER);
	myScene->SetArmInstance(myModel);

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


	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_HIT, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::HEALTH, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::DISPLAY_MARKER, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::DISPLAY_RESPAWN, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::PRESS_E_TEXT, this);

	myPistolProxy = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/Pistol/SK_pistol_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myPistolModel = new Prism::Instance(*myPistolProxy, myInputComponentEyeOrientation, shouldUseSpecialFoV);
	AddWeaponAnimation(ePlayerState::PISTOL_IDLE, "Data/Resource/Model/First_person/Pistol/SK_pistol_idle.fbx", true, true);
	AddWeaponAnimation(ePlayerState::PISTOL_HOLSTER, "Data/Resource/Model/First_person/Pistol/SK_pistol_holster.fbx", false, true);
	AddWeaponAnimation(ePlayerState::PISTOL_DRAW, "Data/Resource/Model/First_person/Pistol/SK_pistol_draw.fbx", false, true);
	AddWeaponAnimation(ePlayerState::PISTOL_RELOAD, "Data/Resource/Model/First_person/Pistol/SK_pistol_reload.fbx", false, true);
	AddWeaponAnimation(ePlayerState::PISTOL_FIRE, "Data/Resource/Model/First_person/Pistol/SK_pistol_fire.fbx", false, true);
	//myPistolModel->Update(1.f / 30.f);

	myShotgunProxy = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/Shotgun/SK_shotgun_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myShotgunModel = new Prism::Instance(*myShotgunProxy, myInputComponentEyeOrientation, shouldUseSpecialFoV);
	AddWeaponAnimation(ePlayerState::SHOTGUN_IDLE, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_idle.fbx", true, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_HOLSTER, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_holster.fbx", false, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_DRAW, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_draw.fbx", false, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_RELOAD, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_reload.fbx", false, true);
	AddWeaponAnimation(ePlayerState::SHOTGUN_FIRE, "Data/Resource/Model/First_person/Shotgun/SK_shotgun_fire.fbx", false, true);
	//myShotgunModel->Update(1.f / 30.f);

	myGrenadeLauncherProxy = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myGrenadeLauncherModel = new Prism::Instance(*myGrenadeLauncherProxy, myInputComponentEyeOrientation, shouldUseSpecialFoV);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_IDLE, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_idle.fbx", true, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_HOLSTER, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_holster.fbx", false, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_DRAW, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_draw.fbx", false, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_RELOAD, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_reload.fbx", false, true);
	AddWeaponAnimation(ePlayerState::GRENADE_LAUNCHER_FIRE, "Data/Resource/Model/First_person/GrenadeLauncher/SK_grenade_launcher_fire.fbx", false, true);
	//myGrenadeLauncherModel->Update(1.f / 30.f);

	myCurrentWeaponModel = myPistolModel;
	//aScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);
	myScene->SetWeaponInstance(myCurrentWeaponModel);

	for (int i = 0; i < 5; ++i)
	{
		Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myWeaponAnimations[i].myData.myFile, "barrel_jnt1", myWeaponAnimations[i].myMuzzleBone);
	}
	for (int i = 5; i < 10; ++i)
	{
		Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myWeaponAnimations[i].myData.myFile, "shotgun_barrel_jnt1", myWeaponAnimations[i].myMuzzleBone);
	}
	for (int i = 10; i < 15; ++i)
	{
		Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myWeaponAnimations[i].myData.myFile, "luncher_barrel_jnt1", myWeaponAnimations[i].myMuzzleBone);
	}
}

FirstPersonRenderComponent::~FirstPersonRenderComponent()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_HIT, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::HEALTH, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::DISPLAY_MARKER, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::DISPLAY_RESPAWN, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::PRESS_E_TEXT, this);
	SAFE_DELETE(myCrosshair);
	SAFE_DELETE(myDamageIndicator);
	SAFE_DELETE(myPickupIndicator);
	SAFE_DELETE(myLowLifeIndicator);
	SAFE_DELETE(my3DGUIManager);
	SAFE_DELETE(myModel);
	SAFE_DELETE(myCoOpSprite);
	SAFE_DELETE(myPistolModel);
	SAFE_DELETE(myShotgunModel);
	SAFE_DELETE(myGrenadeLauncherModel);
	SAFE_DELETE(myCurrentWeaponModel);
	SAFE_DELETE(myPistolProxy);
	SAFE_DELETE(myShotgunProxy);
	SAFE_DELETE(myGrenadeLauncherProxy);
	SAFE_DELETE(myMarker);
	SAFE_DELETE(myIsDeadScreen);
	myCoOpCircles.RemoveAll();
	myCoOpRespawns.RemoveAll();


}

void FirstPersonRenderComponent::Init()
{
	ShootingComponent* shooting = myEntity.GetComponent<ShootingComponent>();
	my3DGUIManager = new GUI::GUIManager3D(myModel, myScene
		, shooting->GetWeapon(eWeaponType::PISTOL)->GetClipSize(), shooting->GetWeapon(eWeaponType::PISTOL)->GetAmmoInClip()
		, shooting->GetWeapon(eWeaponType::SHOTGUN)->GetClipSize(), shooting->GetWeapon(eWeaponType::SHOTGUN)->GetAmmoInClip(), shooting->GetWeapon(eWeaponType::SHOTGUN)->GetAmmoTotal()
		, shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetClipSize(), shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetAmmoInClip(), shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetAmmoTotal());
}

void FirstPersonRenderComponent::Update(float aDelta)
{
	if (myPistolProxy->IsLoaded() == true && myPistolHasUpdated == false)
	{
		myPistolHasUpdated = true;
		myPistolModel->Update(0.f);
	}
	if (myShotgunProxy->IsLoaded() == true && myShotgunHasUpdated == false)
	{
		myShotgunHasUpdated = true;
		myShotgunModel->Update(0.f);
	}
	if (myGrenadeLauncherProxy->IsLoaded() == true && myGrenadeLauncherHasUpdated == false)
	{
		myGrenadeLauncherHasUpdated = true;
		myGrenadeLauncherModel->Update(0.f);
	}

	if (myEntity.GetState() == eEntityState::DIE && myHasDied == false)
	{
		myHasDied = true;
		//myEntity.GetScene()->RemoveInstance(myModel);
		//myScene->SetArmInstance(nullptr);
	}
	if (myEntity.GetState() != eEntityState::DIE && myHasDied == true)
	{
		myHasDied = false;
		//myEntity.GetScene()->AddInstance(myModel, eObjectRoomType::DYNAMIC);
		//myScene->SetArmInstance(myModel);
	}



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
		if (myCurrentState == ePlayerState::PISTOL_RELOAD
			|| myCurrentState == ePlayerState::SHOTGUN_RELOAD
			|| myCurrentState == ePlayerState::GRENADE_LAUNCHER_RELOAD)
		{
			myEntity.GetComponent<ShootingComponent>()->ReloadWeapon();
		}

		if (myCurrentState == ePlayerState::PISTOL_HOLSTER
			|| myCurrentState == ePlayerState::SHOTGUN_HOLSTER
			|| myCurrentState == ePlayerState::GRENADE_LAUNCHER_HOLSTER)
		{
			if (myIntentions[0] == ePlayerState::PISTOL_DRAW)
			{
				myEntity.GetComponent<ShootingComponent>()->SetCurrentWeapon(eWeaponType::PISTOL);
			}
			else if (myIntentions[0] == ePlayerState::SHOTGUN_DRAW)
			{
				myEntity.GetComponent<ShootingComponent>()->SetCurrentWeapon(eWeaponType::SHOTGUN);
			}
			else if (myIntentions[0] == ePlayerState::GRENADE_LAUNCHER_DRAW)
			{
				myEntity.GetComponent<ShootingComponent>()->SetCurrentWeapon(eWeaponType::GRENADE_LAUNCHER);
			}
		}


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

	Prism::AnimationData& weaponData = myWeaponAnimations[int(myCurrentState)].myData;
	if (myCurrentWeaponModel->IsAnimationDone() == false || weaponData.myShouldLoop == true)
	{
		myCurrentWeaponModel->Update(aDelta);

	}
	weaponData.myElapsedTime += aDelta;

	UpdateJoints();
	if (myDisplayDamageIndicatorTimer >= 0.f)
	{
		myDisplayDamageIndicatorTimer -= aDelta;
	}

	if (myDisplayHealthIndicatorTimer >= 0.f)
	{
		myDisplayHealthIndicatorTimer -= aDelta;
	}

	if (myDisplayUpgradeIndicatorTimer >= 0.f)
	{
		myDisplayUpgradeIndicatorTimer -= aDelta;
	}

	my3DGUIManager->Update(myUIJoint, myHealthJoint, myCurrentHealth
		, myMaxHealth, aDelta, myEntity.GetComponent<ShootingComponent>()->GetCurrentWeapon()->GetWeaponType());


}

void FirstPersonRenderComponent::UpdateCoOpPositions(const CU::GrowingArray<Entity*>& somePlayers)
{
	if (somePlayers.Size() > 0)
	{
		myCoOpCircles[0].myPosition = somePlayers[0]->GetOrientation().GetPos();
		myIsCoOp = true;
	}
}

void FirstPersonRenderComponent::Render(Prism::Texture* aArmDepthTexture, bool aIsFirstLevel)
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);

	if (myDisplayDamageIndicatorTimer > 0.f)
	{
		float ratio = myDisplayDamageIndicatorTimer / myDisplayDamageIndicatorTimerMax;
		myDamageIndicator->Render(windowSize * 0.5f, CU::Vector2<float>(1.f, 1.f) + (1.f - ratio) * 0.1f, CU::Vector4<float>(1.f, 1.f, 1.f, ratio));
	}
	else if (myDisplayHealthIndicatorTimer > 0.f)
	{
		CU::Vector4<float> color = HEALTH_COLOR;
		color.w = fminf(fminf(1.f, myDisplayHealthIndicatorTimer), myDisplayPickupTime - myDisplayHealthIndicatorTimer);
		myPickupIndicator->Render(windowSize * 0.5f, { 1.f, 1.f }, color);
	}
	else if (myDisplayUpgradeIndicatorTimer > 0.f)
	{
		CU::Vector4<float> color = UPGRADE_COLOR;
		color.w = fminf(fminf(1.f, myDisplayUpgradeIndicatorTimer), myDisplayPickupTime - myDisplayUpgradeIndicatorTimer);
		myPickupIndicator->Render(windowSize * 0.5f, { 1.f, 1.f }, color);
	}

	float lifePercentage = float(myCurrentHealth) / float(myMaxHealth);

	if (lifePercentage < 0.4f)
	{
		myLowLifeIndicator->Render(windowSize * 0.5f, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f - lifePercentage });
	}

	Prism::Engine::GetInstance()->SetDepthStencil(aArmDepthTexture->GetDepthStencilView());
	my3DGUIManager->Render(aIsFirstLevel);
	Prism::Engine::GetInstance()->RestoreDepthStencil();

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
			Prism::Engine::GetInstance()->PrintText(std::to_string(int(toBuddy)) + " m", { newRenderPos.x, newRenderPos.y }, Prism::eTextType::RELEASE_TEXT);
		}
	}

	if (myIsCoOp == true)
	{
		for (int i = 0; i < myCoOpCircles.Size(); ++i)
		{
			CU::Matrix44<float> renderPos;
			CU::Vector3<float> tempPos(myCoOpCircles[i].myPosition);
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

			float color = myCoOpCircles[i].myLifePercentage;

			if (myCoOpCircles[i].myLifePercentage <= 0.f)
			{
				myCoOpSprite->Render({ newRenderPos.x, newRenderPos.y - 0.5f }, { 1.f, 1.f }, { 1.f - color, color, color, 1.f });
				Prism::Engine::GetInstance()->PrintText(myCoOpRespawns[i].myCurrentValue, { newRenderPos.x, newRenderPos.y - 0.5f }, Prism::eTextType::RELEASE_TEXT);
			}
			else
			{
				myCoOpSprite->Render({ newRenderPos.x, newRenderPos.y }, { 1.f, 1.f }, { 1.f - color, color, color, 1.f });
			}
		}
	}

	for (int i = 0; i < myPressETexts.Size(); ++i)
	{
		CU::Matrix44<float> renderPos;
		CU::Vector3<float> tempPos(myPressETexts[i].myPosition);
		float lengthToText = CU::Length(tempPos - myInputComponentEyeOrientation.GetPos());
		if (lengthToText > 10.f)
		{
			continue;
		}

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
		newRenderPos.x -= 70.f;

		//myCoOpSprite->Render({ newRenderPos.x, newRenderPos.y });
		Prism::Engine::GetInstance()->PrintText("Press E", { newRenderPos.x, newRenderPos.y }, Prism::eTextType::RELEASE_TEXT, 2.f, CU::Vector4<float>(1.f, 1.f, 1.f, 1.f - (lengthToText / 10.f)));
	}

	if (myEntity.GetState() == eEntityState::DIE)
	{
		myIsDeadScreen->Render(windowSize * 0.5f);
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

const CU::Matrix44<float>& FirstPersonRenderComponent::GetMuzzleJointOrientation() const
{
	return myMuzzleJoint;
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
	myWeaponAnimations[int(aState)].myData = newData;
}

void FirstPersonRenderComponent::PlayAnimation(ePlayerState aAnimationState)
{
	myCurrentState = aAnimationState;
	if (myCurrentState == ePlayerState::PISTOL_DRAW)
	{
		//myScene->RemoveInstance(myCurrentWeaponModel);
		myCurrentWeaponModel = myPistolModel;
		//myScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);

		myScene->SetWeaponInstance(myCurrentWeaponModel);
	}
	else if (myCurrentState == ePlayerState::SHOTGUN_DRAW)
	{
		//myScene->RemoveInstance(myCurrentWeaponModel);
		myCurrentWeaponModel = myShotgunModel;
		//myScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);

		myScene->SetWeaponInstance(myCurrentWeaponModel);
	}
	else if (myCurrentState == ePlayerState::GRENADE_LAUNCHER_DRAW)
	{
		//myScene->RemoveInstance(myCurrentWeaponModel);
		myCurrentWeaponModel = myGrenadeLauncherModel;
		//myScene->AddInstance(myCurrentWeaponModel, eObjectRoomType::ALWAYS_RENDER);

		myScene->SetWeaponInstance(myCurrentWeaponModel);
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

	Prism::AnimationData& weaponData = myWeaponAnimations[int(aAnimationState)].myData;
	myCurrentWeaponModel->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(weaponData.myFile.c_str()));

	if (weaponData.myResetTimeOnRestart == true)
	{
		myCurrentWeaponModel->ResetAnimationTime(0.f);
	}
	else
	{
		myCurrentWeaponModel->ResetAnimationTime(weaponData.myElapsedTime);
	}

	PlayCorrectSound();
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
		myDisplayDamageIndicatorTimer = myDisplayDamageIndicatorTimerMax;
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_TakeDamage", 0);
	}
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessageHealth& aMessage, const sockaddr_in&)
{
	if (aMessage.myGID == myEntity.GetGID())
	{
		if (myCurrentHealth < aMessage.myCurrentHealth)
		{
			myDisplayHealthIndicatorTimer = myDisplayPickupTime;
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_HealthPickup", 0);
		}


		myMaxHealth = aMessage.myMaxHealth;
		myCurrentHealth = aMessage.myCurrentHealth;

		if (myCurrentHealth == 0)
		{
			myDisplayHealthIndicatorTimer = 0.f;
		}
	}
	else
	{
		myCoOpCircles[0].myLifePercentage = float(aMessage.myCurrentHealth) / aMessage.myMaxHealth;
	}
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessageDisplayMarker& aMessage, const sockaddr_in&)
{
	myRenderMarker = aMessage.myDisplayMarker;
	myMarkerPosition = aMessage.myPosition;
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessageDisplayRespawn& aMessage, const sockaddr_in&)
{
	for (int i = 0; i < myCoOpRespawns.Size(); i++)
	{
		if (myCoOpRespawns[i].myGID == aMessage.myGID)
		{
			myCoOpRespawns.RemoveCyclicAtIndex(i);
			break;
		}
	}

	if (aMessage.myDisplayRespawn == true)
	{
		CoOpRespawn respawn;
		respawn.myMaxValue = aMessage.myMaxValue;
		respawn.myCurrentValue = aMessage.myCurrentValue;
		respawn.myPosition = aMessage.myPosition;
		respawn.myGID = aMessage.myGID;
		myCoOpRespawns.Add(respawn);
	}
}

void FirstPersonRenderComponent::ReceiveNetworkMessage(const NetMessagePressEText& aMessage, const sockaddr_in&)
{
	if (aMessage.myShouldAdd == true)
	{
		PressEText text;
		text.myGID = aMessage.myGID;
		text.myPosition = aMessage.myPosition;
		myPressETexts.Add(text);
	}
	else
	{
		for (int i = myPressETexts.Size() - 1; i >= 0; --i)
		{
			if (myPressETexts[i].myGID == aMessage.myGID)
			{
				myPressETexts.RemoveCyclicAtIndex(i);
				break;
			}
		}
	}
}

void FirstPersonRenderComponent::OnResize(CU::Vector2<float> aNewSize)
{
	CU::Vector2<float> hotspot = aNewSize / 2.f;

	myDamageIndicator->SetSize(aNewSize, hotspot);
	myPickupIndicator->SetSize(aNewSize, hotspot);
	myLowLifeIndicator->SetSize(aNewSize, hotspot);
	myIsDeadScreen->SetSize(aNewSize, hotspot);
}

void FirstPersonRenderComponent::ReceiveNote(const UpgradeNote& aNote)
{
	if (aNote.myData.myClipSizeModifier > 0)
	{
		my3DGUIManager->Rebuild(aNote.myData.myWeaponType, aNote.myData.myClipSizeModifier);
	}
	myDisplayUpgradeIndicatorTimer = myDisplayPickupTime;
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

	WeaponAnimationData& currentWeaponAnimation = myWeaponAnimations[static_cast<int>(myCurrentState)];
	if (currentWeaponAnimation.IsValid() == true)
	{
		//if (myFirstTimeActivateAnimation == false)
		//{
		//	myFirstTimeActivateAnimation = true;
		//	//PlayAnimation(ePlayerState::PISTOL_IDLE);
		//	AddIntention(ePlayerState::PISTOL_FIRE, true);
		//}

		myMuzzleJoint = CU::InverseSimple(*currentWeaponAnimation.myMuzzleBone.myBind) * (*currentWeaponAnimation.myMuzzleBone.myJoint) * myInputComponentEyeOrientation;
	}
}

void FirstPersonRenderComponent::PlayCorrectSound()
{
	switch (myCurrentState)
	{
	case ePlayerState::PISTOL_DRAW:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_DrawPistol", 0);
		break;
	case ePlayerState::PISTOL_HOLSTER:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_HolsterPistol", 0);
		break;
	case ePlayerState::SHOTGUN_DRAW:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_DrawShotgun", 0);
		break;
	case ePlayerState::SHOTGUN_HOLSTER:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_HolsterShotgun", 0);
		break;
	case ePlayerState::GRENADE_LAUNCHER_DRAW:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_DrawGrenadeLauncher", 0);
		break;
	case ePlayerState::GRENADE_LAUNCHER_HOLSTER:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_HolsterGrenadeLauncher", 0);
		break;
	default:
		break;
	}
}