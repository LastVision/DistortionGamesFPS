#include "stdafx.h"

#include "Animation.h"
#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Instance.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "ModelProxy.h"
#include "InstancingHelper.h"

Prism::Instance::Instance(ModelProxy& aModel, const CU::Matrix44<float>& anOrientation, eOctreeType anOctreeType
		, const float& aObjectCullingRadius, bool aAlwaysRender, bool aCastShadow)
	: myProxy(aModel)
	, myOctreeType(anOctreeType)
	, myOrientation(anOrientation)
	, myScale({1,1,1})
	, myObjectCullingRadius(aObjectCullingRadius)
	, myHierarchyIsBuilt(false)
	, myShouldRender(true)
	, mySelectionColor(0.f, 0.f, 1.f, 0.1f)
	, myAnimation(nullptr)
	, myTotalTime(0.f)
	, myOwnerType(eOwnerType::NOT_USED)
	, myRenderThroughCulling(true)
	, myAlwaysRender(aAlwaysRender)
	, myCastShadow(aCastShadow)
{
}

Prism::Instance::~Instance()
{
}

void Prism::Instance::Update(float aDelta)
{
	if (myProxy.IsLoaded() == true && myProxy.IsAnimated())
	{
		//if (myShouldRender == true) // creates bug where enemy units are not animated when arriving from Fog of war
		//{
			if (myHierarchyIsBuilt == false)
			{
				if (myProxy.myModelAnimated->myAnimation != nullptr)
				{
					myAnimation = myProxy.myModelAnimated->myAnimation;
				}
				//MemoryTracker::GetInstance()->SetRunTime(false);
				BuildHierarchy(myHierarchy, myProxy.myModelAnimated);
				//MemoryTracker::GetInstance()->SetRunTime(true);
				myHierarchyIsBuilt = true;

			}

			myHierarchy.Update(aDelta, myAnimation->GetAnimationLenght());
			if (myAnimation != nullptr)
			{
				myAnimation->Update(myTotalTime, myBones);
			}
		//}

		myTotalTime += aDelta;
	}
}

void Prism::Instance::Render(const Camera& aCamera, bool aIsDepthRender)
{
	if (myShouldRender == false)
	{
		return;
	}

	if (myProxy.IsLoaded())
	{

		myProxy.GetEffect()->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myProxy.GetEffect()->SetScaleVector(myScale);
		myProxy.GetEffect()->SetCameraPosition(aCamera.GetOrientation().GetPos());

		if (myProxy.IsAnimated() == true)
		{
			myProxy.myModelAnimated->ActivateAlbedo(myOwnerType);
			myProxy.GetEffect()->SetBones(myBones);
			RenderModelAnimated(myProxy.myModelAnimated, myOrientation, aCamera, myHierarchy, aIsDepthRender);
		}
		else
		{
			myProxy.myModel->ActivateAlbedo(myOwnerType);
			myProxy.Render(myOrientation, aCamera.GetOrientation().GetPos(), aIsDepthRender);
		}
	}
}

void Prism::Instance::Render(const Camera& aCamera, InstancingHelper& aInstancingHelper, bool aIsDepthRender)
{
	if (myShouldRender == false)
	{
		return;
	}

	if (myProxy.IsLoaded())
	{
		if (myProxy.IsAnimated() == true)
		{
			myProxy.GetEffect()->SetViewProjectionMatrix(aCamera.GetViewProjection());
			myProxy.GetEffect()->SetScaleVector(myScale);
			myProxy.GetEffect()->SetCameraPosition(aCamera.GetOrientation().GetPos());

			myProxy.myModelAnimated->ActivateAlbedo(myOwnerType);
			myProxy.GetEffect()->SetBones(myBones);
			RenderModelAnimated(myProxy.myModelAnimated, myOrientation, aCamera, myHierarchy, aIsDepthRender);
		}
		else
		{
			Model* toRender = myProxy.myModel->GetRealModel(myOrientation.GetPos(), aCamera.GetOrientation().GetPos());
			aInstancingHelper.AddModel(myOwnerType, toRender, myOrientation, myScale, myOrientation.GetPos().y);
		}
	}
}


void Prism::Instance::RenderModelAnimated(ModelAnimated* aModel, const CU::Matrix44<float>& aParent
	, const Camera& aCamera, TransformationNodeInstance& aHierarchy, bool aIsDepthRender)
{
	if (aModel->myIsNULLObject == false)
	{
		aModel->Render(aHierarchy.GetTransformation() * aParent, aCamera.GetOrientation().GetPos(), aIsDepthRender);
	}

	for (int i = 0; i < aHierarchy.GetChildren().Size(); ++i)
	{
		DL_ASSERT_EXP(aModel->myChildren[i] != nullptr, "Missmatch number of TransformationNodes and number of Models");

		RenderModelAnimated(aModel->myChildren[i], aHierarchy.GetTransformation() * aParent, aCamera, *aHierarchy.GetChildren()[i], aIsDepthRender);
	}
}

CU::Vector3<float> Prism::Instance::GetPosition() const
{
	return myOrientation.GetPos();
}

void Prism::Instance::SetEffect(const std::string& aEffectFile)
{
	if (myProxy.IsLoaded())
	{
		myProxy.SetEffect(EffectContainer::GetInstance()->GetEffect(aEffectFile));
	}
}

void Prism::Instance::SetScale(const CU::Vector3<float>& aScaleVector)
{
	myScale = aScaleVector;
	
}

void Prism::Instance::UpdateDirectionalLights(
	const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->UpdateDirectionalLights(someDirectionalLightData);
	}
}

void Prism::Instance::UpdatePointLights(
	const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->UpdatePointLights(somePointLightData);
	}
}

void Prism::Instance::UpdateSpotLights(
	const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->UpdateSpotLights(someSpotLightData);
	}
}

void Prism::Instance::SetAnimation(Animation* aAnimation)
{
	myAnimation = aAnimation;
}

Prism::Animation* Prism::Instance::GetCurrentAnimation() const
{
	return myAnimation;
}

bool Prism::Instance::IsAnimationDone()
{
	if (myAnimation == nullptr) return true;

	if (myTotalTime > myAnimation->GetAnimationLenght()) return true;

	return false;
}

void Prism::Instance::ResetAnimationTime(float aTime)
{
	myTotalTime = aTime;
}

void Prism::Instance::ActivateAlbedo(eOwnerType aOwner)
{
	myOwnerType = aOwner;
}

void Prism::Instance::SetShouldRender(bool aStatus)
{
	myShouldRender = aStatus;
}

bool Prism::Instance::GetShouldRender() const
{
	return myShouldRender;
}

void Prism::Instance::SetRenderThroughCulling(bool aStatus)
{
	myRenderThroughCulling = aStatus;
}

bool Prism::Instance::GetRenderThroughCulling() const
{
	return myRenderThroughCulling;
}

bool Prism::Instance::GetAlwaysRender() const
{
	return myAlwaysRender;
}

bool Prism::Instance::GetCastShadow() const
{
	return myCastShadow;
}

Prism::ModelProxy& Prism::Instance::GetModel()
{
	return myProxy;
}

void Prism::Instance::BuildHierarchy(TransformationNodeInstance& aHierarchy, ModelAnimated* aModel)
{
	aHierarchy.SetTransformationNode(aModel->myTransformation);

	for (int i = 0; i < aModel->myChildTransforms.Size(); ++i)
	{
		TransformationNodeInstance* newNode = new TransformationNodeInstance();
		newNode->SetTransformationNode(aModel->myChildTransforms[i]);
		aHierarchy.AddChildNode(newNode);

		BuildHierarchy(*newNode, aModel->myChildren[i]);
	}
}
