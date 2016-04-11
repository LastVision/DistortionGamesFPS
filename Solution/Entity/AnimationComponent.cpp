#include "stdafx.h"
#include <InputWrapper.h>
#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include <Effect.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <Scene.h>
#include <Texture.h>

//#define BOX_MODE

AnimationComponent::AnimationComponent(Entity& aEntity, const AnimationComponentData& aComponentData, Prism::Scene* aScene)
	: Component(aEntity)
	, myComponentData(aComponentData)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
	, myIsEnemy(false)
	, myHasSetCalcedMuzzle(false)
	, myMuzzleflashTimer(0.f)
	, myCurrentMuzzleflash(0)
{
#ifndef BOX_MODE
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(myComponentData.myModelPath
		, myComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation);

	int animations = static_cast<int>(eEntityState::_COUNT);

	for (int i = 0; i < animations; ++i)
	{
		AnimationLoadData loadAnimation = myComponentData.myAnimations[i];
		AddAnimation(loadAnimation.myEntityState, loadAnimation.myAnimationPath, loadAnimation.myLoopFlag, loadAnimation.myResetTimeOnRestart);
	}
	
	if (myEntity.GetType() == eEntityType::UNIT && myEntity.GetSubType() != "player")
	{
		myIsEnemy = true;
		for (int i = 0; i < animations; ++i)
		{
			Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myAnimations[i].myFile, "r_hand_wpn_tip12", myEnemyAnimations[i].myWeaponBone);
		}
		Prism::ModelProxy* weapon = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/Enemy_weapon/SK_enemy_weapon_rifle.fbx", myComponentData.myEffectPath);
		myWeapon = new Prism::Instance(*weapon, myWeaponJoint);

		Prism::ModelLoader::GetInstance()->GetHierarchyToBone("Data/Resource/Model/Enemy_weapon/SK_enemy_weapon_rifle.fbx", "weapon_muzzle_tip-1", myMuzzleBone);
	}
	else
	{
		Prism::ModelLoader::GetInstance()->GetHierarchyToBone("Data/Resource/Model/Player/SK_player_fire.fbx", "r_wrist_jnt11", myMuzzleBone);
	}

	LoadMuzzleFlashes(aScene);
#endif
}

AnimationComponent::~AnimationComponent()
{
	myEntity.GetScene()->RemoveInstance(myInstance);
	SAFE_DELETE(myInstance);
	SAFE_DELETE(myWeapon);

	SAFE_DELETE(myMuzzleflash[0]);
	SAFE_DELETE(myMuzzleflash[1]);
	SAFE_DELETE(myMuzzleflash[2]);
	SAFE_DELETE(myMuzzleflash[3]);
	SAFE_DELETE(myMuzzleflash[4]);
}

void AnimationComponent::Reset()
{
	myPrevEntityState = myEntity.GetState();
	PlayAnimation(myEntity.GetState());
}

void AnimationComponent::AddAnimation(eEntityState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)] = newData;
}

void AnimationComponent::Update(float aDeltaTime)
{
	if (myEntity.IsActive() == false && myEntity.GetState() == eEntityState::DIE)
	{
		int apa;
		apa = 5;
	}

	for (int i = 0; i < 5; ++i)
	{
		myMuzzleflash[i]->SetShouldRender(false);
	}

	if (myMuzzleBone.IsValid() == true && myHasSetCalcedMuzzle == false)
	{
		myHasSetCalcedMuzzle = true;
		myMuzzleBoneCalced = CU::InverseSimple(*myMuzzleBone.myBind) * (*myMuzzleBone.myJoint);
	}

	AnimationData& data = myAnimations[int(myEntity.GetState())];
	if (myPrevEntityState != myEntity.GetState())
	{
		PlayAnimation(myEntity.GetState());
		myPrevEntityState = myEntity.GetState();
	}

	if (myInstance->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myInstance->Update(aDeltaTime);
	}

	data.myElapsedTime += aDeltaTime;

	if (myIsEnemy == true)
	{
		//myWeapon->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));
		myWeapon->Update(aDeltaTime);
		EnemyAnimationBone currentAnimation = myEnemyAnimations[int(myEntity.GetState())];
		if (currentAnimation.IsValid())
		{
			myWeaponJoint = CU::InverseSimple(*currentAnimation.myWeaponBone.myBind) * (*currentAnimation.myWeaponBone.myJoint) * myEntity.GetOrientation();

			myMuzzleOrientation = myMuzzleBoneCalced * myWeaponJoint;
		}
	}
	else
	{
		myMuzzleOrientation = myEntity.GetOrientation();
		if (myMuzzleBone.IsValid())
		{
			CU::Vector3<float> offset(-0.04535f, 0.06111f, 0.1309f);

			myMuzzleOrientation.SetPos((CU::InverseSimple(*myMuzzleBone.myBind) * (*myMuzzleBone.myJoint) *myEntity.GetOrientation()).GetPos() + offset * myEntity.GetOrientation());
		}
	}



	myMuzzleflashTimer -= aDeltaTime;
	if (myMuzzleflashTimer > 0.f)
	{
		int prev = myCurrentMuzzleflash;
		myCurrentMuzzleflash = rand() % 5;
		if (prev == myCurrentMuzzleflash)
		{
			++myCurrentMuzzleflash;
			if (myCurrentMuzzleflash > 4)
			{
				myCurrentMuzzleflash = 0;
			}
		}
		myMuzzleflash[myCurrentMuzzleflash]->SetShouldRender(true);
	}
	//Prism::DebugDrawer::GetInstance()->RenderLine3D(CU::Vector3<float>(), myMuzzleOrientation.GetPos());

	if (myEntity.GetState() == eEntityState::ATTACK && IsCurrentAnimationDone() == true)
	{
		myEntity.SetState(eEntityState::IDLE);
	}
}

bool AnimationComponent::IsCurrentAnimationDone() const
{
	return myInstance->IsAnimationDone();
}

void AnimationComponent::RestartCurrentAnimation()
{
	myInstance->ResetAnimationTime(0.f);
}

void AnimationComponent::PlayAnimation(eEntityState aAnimationState)
{
	AnimationData& data = myAnimations[int(aAnimationState)];
	myInstance->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));

	if (data.myResetTimeOnRestart == true)
	{
		myInstance->ResetAnimationTime(0.f);
	}
	else
	{
		myInstance->ResetAnimationTime(data.myElapsedTime);
	}
}

void AnimationComponent::SetRotation(const CU::Vector3<float>& aRotation)
{
	CU::Vector3f position(myEntity.myOrientation.GetPos());
	myEntity.myOrientation.SetPos(CU::Vector3f());

	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundX(aRotation.x) * myEntity.myOrientation;
	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundY(aRotation.y) * myEntity.myOrientation;
	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundZ(aRotation.z) * myEntity.myOrientation;

	myEntity.myOrientation.SetPos(position);
}

void AnimationComponent::SetScale(const CU::Vector3<float>& aScale)
{
	myInstance->SetScale(aScale);
}

Prism::Animation* AnimationComponent::GetCurrent() const
{
	return myInstance->GetCurrentAnimation();
}

void AnimationComponent::AddWeaponToScene(Prism::Scene* aScene)
{
	if (myIsEnemy == true)
	{
		aScene->AddInstance(myWeapon, eObjectRoomType::DYNAMIC);
	}
}

void AnimationComponent::RemoveWeaponFromScene(Prism::Scene* aScene)
{
	if (myIsEnemy == true)
	{
		aScene->RemoveInstance(myWeapon);
	}
}

void AnimationComponent::PlayMuzzleFlash()
{
	myMuzzleflashTimer = 0.2f;
	myMuzzleflash[myCurrentMuzzleflash]->SetShouldRender(true);
}

void AnimationComponent::StopMuzzleFlash()
{
	for (int i = 0; i < 5; ++i)
	{
		myMuzzleflash[i]->SetShouldRender(false);
	}
	myMuzzleflashTimer = 0.f;
	myCurrentMuzzleflash = 0;
}

void AnimationComponent::LoadMuzzleFlashes(Prism::Scene* aScene)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash0.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[0] = new Prism::Instance(*model, myMuzzleOrientation);
	aScene->AddInstance(myMuzzleflash[0], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash1.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[1] = new Prism::Instance(*model, myMuzzleOrientation);
	aScene->AddInstance(myMuzzleflash[1], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash2.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[2] = new Prism::Instance(*model, myMuzzleOrientation);
	aScene->AddInstance(myMuzzleflash[2], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash3.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[3] = new Prism::Instance(*model, myMuzzleOrientation);
	aScene->AddInstance(myMuzzleflash[3], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash4.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[4] = new Prism::Instance(*model, myMuzzleOrientation);
	aScene->AddInstance(myMuzzleflash[4], eObjectRoomType::ALWAYS_RENDER);

	for (int i = 0; i < 5; ++i)
	{
		myMuzzleflash[i]->SetShouldRender(false);
	}
}