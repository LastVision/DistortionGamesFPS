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
#include <Terrain.h>
#include <Texture.h>

//#define BOX_MODE

AnimationComponent::AnimationComponent(Entity& aEntity, AnimationComponentData& aComponentData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
	, myTerrain(aTerrain)
	, myPrevEntityState(myEntity.GetState())
{
#ifndef BOX_MODE
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(aComponentData.myModelPath
		, aComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);
	myInstance->ActivateAlbedo(myEntity.GetOwner());

	int animations = static_cast<int>(eEntityState::_COUNT);

	if (aEntity.myUnitType != eUnitType::GRUNT)
	{
		--animations;
	}

	if (aEntity.GetUnitType() == eUnitType::NOT_A_UNIT)
	{
		animations = 1;
	}

	for (int i = 0; i < animations; ++i)
	{
		AnimationLoadData loadAnimation = aComponentData.myAnimations[i];
		AddAnimation(loadAnimation.myEntityState, loadAnimation.myAnimationPath, loadAnimation.myLoopFlag, loadAnimation.myResetTimeOnRestart);
	}
#endif
}

AnimationComponent::~AnimationComponent()
{
#ifndef BOX_MODE
	if (myEntity.myIsInScene == true && myEntity.GetOctreeType() != Prism::eOctreeType::NOT_IN_OCTREE
		&& myEntity.GetOwner() != eOwnerType::PLAYER)
	{
		myEntity.GetScene().RemoveInstance(myInstance);
	}
	delete myInstance;
	myInstance = nullptr;
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
	//	myInstance->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));
	//
	//	if (data.myResetTimeOnRestart == true)
	//	{
	//		myInstance->ResetAnimationTime(0.f);
	//	}
	//	else
	//	{
	//		myInstance->ResetAnimationTime(data.myElapsedTime);
	//	}
	//}
	
	if (myInstance->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myInstance->Update(aDeltaTime);
	}
	

	if (myEntity.GetTemporaryOwner() != eOwnerType::NOT_USED)
	{
		myInstance->ActivateAlbedo(myEntity.GetTemporaryOwner());
	}

	data.myElapsedTime += aDeltaTime;
	//myPrevEntityState = myEntity.GetState();
#else
	CU::Vector3<float> pos = myTerrain.GetHeight(myEntity.GetOrientation().GetPos(), 2.f);
	Prism::RenderBox(pos, eColorDebug::BLUE, 0.5f);
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