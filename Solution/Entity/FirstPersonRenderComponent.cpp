#include "stdafx.h"

#include <Animation.h>
#include <AnimationSystem.h>
#include <Engine.h>
#include "Entity.h"
#include "FirstPersonRenderComponent.h"
#include <GUIManager3D.h>
#include "HealthComponent.h"
#include <Instance.h>
#include "InputComponent.h"
#include <ModelLoader.h>
#include <Scene.h>
#include "ShootingComponent.h"
#include <SpriteProxy.h>

FirstPersonRenderComponent::FirstPersonRenderComponent(Entity& aEntity, Prism::Scene* aScene)
	: Component(aEntity)
	, myInputComponentEyeOrientation(myEntity.GetComponent<InputComponent>()->GetEyeOrientation())
	, myCurrentState(ePlayerState::PISTOL_IDLE)
	, myIntentions(8)
{
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);

	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myModel = new Prism::Instance(*model, myInputComponentEyeOrientation);
	aScene->AddInstance(myModel, true);

	AddAnimation(ePlayerState::PISTOL_IDLE, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", true, true);
	AddAnimation(ePlayerState::PISTOL_HOLSTER, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_DRAW, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_RELOAD, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_FIRE, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);

	AddAnimation(ePlayerState::SHOTGUN_IDLE, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", true, true);
	AddAnimation(ePlayerState::SHOTGUN_HOLSTER, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::SHOTGUN_DRAW, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::SHOTGUN_RELOAD, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::SHOTGUN_FIRE, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);

	AddAnimation(ePlayerState::GRENADE_LAUNCHER_IDLE, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", true, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_HOLSTER, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_DRAW, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_RELOAD, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);
	AddAnimation(ePlayerState::GRENADE_LAUNCHER_FIRE, "Data/Resource/Model/First_person/SK_38Bone_animationTest.fbx", false, true);

	myModel->Update(1.f / 30.f);

	PlayAnimation(ePlayerState::PISTOL_IDLE);
	while (myModel->GetCurrentAnimation() == nullptr)
	{

	}

	for (int i = 0; i < static_cast<int>(ePlayerState::_COUNT); ++i)
	{
		myAnimations[i].myUIBone = Prism::AnimationSystem::GetInstance()->GetAnimation(myAnimations[i].myData.myFile.c_str())->GetHiearchyToBone("ui_jnt2");
		myAnimations[i].myHealthBone = Prism::AnimationSystem::GetInstance()->GetAnimation(myAnimations[i].myData.myFile.c_str())->GetHiearchyToBone("health_jnt2");
	}

	ShootingComponent* shooting = myEntity.GetComponent<ShootingComponent>();
	my3DGUIManager = new GUI::GUIManager3D(myModel, aScene
		, shooting->GetWeapon(eWeaponType::PISTOL)->GetClipSize(), shooting->GetWeapon(eWeaponType::PISTOL)->GetAmmoInClip()
		, shooting->GetWeapon(eWeaponType::SHOTGUN)->GetClipSize(), shooting->GetWeapon(eWeaponType::SHOTGUN)->GetAmmoInClip()
		, shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetClipSize(), shooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetAmmoInClip());

}


FirstPersonRenderComponent::~FirstPersonRenderComponent()
{
	SAFE_DELETE(myCrosshair);
	SAFE_DELETE(my3DGUIManager);
	SAFE_DELETE(myModel);
}


void FirstPersonRenderComponent::Update(float aDelta)
{
	CU::Matrix44<float> uiJoint = CU::InverseSimple(*myAnimations[static_cast<int>(myCurrentState)].myUIBone.myBind)
		* (*myAnimations[static_cast<int>(myCurrentState)].myUIBone.myJoint) * myInputComponentEyeOrientation;

	CU::Matrix44<float> healthJoint = CU::InverseSimple(*myAnimations[static_cast<int>(myCurrentState)].myHealthBone.myBind)
		* (*myAnimations[static_cast<int>(myCurrentState)].myHealthBone.myJoint) * myInputComponentEyeOrientation;

	my3DGUIManager->Update(uiJoint, healthJoint, myEntity.GetComponent<HealthComponent>()->GetCurrentHealth()
		, myEntity.GetComponent<HealthComponent>()->GetMaxHealth(), aDelta);



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
	if (myModel->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myModel->Update(aDelta);
	}
	if (myModel->IsAnimationDone() == true && data.myShouldLoop == false)
	{
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
	data.myElapsedTime += aDelta;
}

void FirstPersonRenderComponent::Render()
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);
	my3DGUIManager->Render();
}

bool FirstPersonRenderComponent::IsCurrentAnimationDone() const
{
	return myModel->IsAnimationDone();
}

void FirstPersonRenderComponent::RestartCurrentAnimation()
{
	myModel->ResetAnimationTime(0.f);
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

void FirstPersonRenderComponent::PlayAnimation(ePlayerState aAnimationState)
{
	myCurrentState = aAnimationState;
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
			myEntity.GetComponent<FirstPersonRenderComponent>()->PlayAnimation(myCurrentState);
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			myCurrentState = ePlayerState::GRENADE_LAUNCHER_IDLE;
			myEntity.GetComponent<FirstPersonRenderComponent>()->PlayAnimation(myCurrentState);
			break;
		case eWeaponType::SHOTGUN:
			myCurrentState = ePlayerState::SHOTGUN_IDLE;
			myEntity.GetComponent<FirstPersonRenderComponent>()->PlayAnimation(myCurrentState);
			break;
		}
	}
	myIntentions.Add(aPlayerState);
}