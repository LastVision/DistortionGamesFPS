#include "stdafx.h"

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

AnimationComponent::AnimationComponent(Entity& aEntity, const AnimationComponentData& aComponentData)
	: Component(aEntity)
	, myComponentData(aComponentData)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
	, myIsEnemy(false)
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
	
	if (myEntity.GetType() == eEntityType::UNIT && myEntity.GetSubType() == "gundroid")
	{
		myIsEnemy = true;
		for (int i = 0; i < animations; ++i)
		{
			Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myAnimations[i].myFile, "r_hand_wpn_tip12", myEnemyAnimations[i].myWeaponBone);
		}
		Prism::ModelProxy* weapon = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/Enemy_weapon/SK_enemy_weapon_rifle.fbx", myComponentData.myEffectPath);
		myWeapon = new Prism::Instance(*weapon, myWeaponJoint);
		
	}

#endif
}

AnimationComponent::~AnimationComponent()
{
#ifndef BOX_MODE

	myEntity.GetScene()->RemoveInstance(myInstance);
	SAFE_DELETE(myInstance);
	SAFE_DELETE(myWeapon);
#endif
}

void AnimationComponent::Reset()
{
	myPrevEntityState = myEntity.GetState();
	PlayAnimation(myEntity.GetState());
}

void AnimationComponent::AddAnimation(eEntityState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
#ifndef BOX_MODE
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)] = newData;
#endif
}

void AnimationComponent::Update(float aDeltaTime)
{
#ifndef BOX_MODE
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
		myWeaponJoint = CU::InverseSimple(*currentAnimation.myWeaponBone.myBind) * (*currentAnimation.myWeaponBone.myJoint) * myEntity.GetOrientation();
	}
#endif
}

bool AnimationComponent::IsCurrentAnimationDone() const
{
#ifndef BOX_MODE
	return myInstance->IsAnimationDone();
#else
	return true;
#endif
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