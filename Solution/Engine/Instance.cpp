#include "stdafx.h"

#include "Animation.h"
#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Frustum.h"
#include "Instance.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "ModelProxy.h"
#include "InstancingHelper.h"

namespace Prism
{
	Instance::Instance(ModelProxy& aModel, const CU::Matrix44<float>& anOrientation)
		: myProxy(aModel)
		, myOrientation(anOrientation)
		, myScale({ 1, 1, 1 })
		, myHierarchyIsBuilt(false)
		, myAnimation(nullptr)
		, myTotalTime(0.f)
		, myShouldRender(true)
	{
	}

	Instance::~Instance()
	{
	}

	void Instance::Update(float aDelta)
	{
		if (myProxy.IsLoaded() == true && myProxy.IsAnimated())
		{
			if (myHierarchyIsBuilt == false)
			{
				if (myProxy.myModelAnimated->myAnimation != nullptr)
				{
					myAnimation = myProxy.myModelAnimated->myAnimation;
				}
				BuildHierarchy(myHierarchy, myProxy.myModelAnimated);
				myHierarchyIsBuilt = true;

			}

			myHierarchy.Update(aDelta, myAnimation->GetAnimationLenght());
			if (myAnimation != nullptr)
			{
				myAnimation->Update(myTotalTime, myBones);
			}

			myTotalTime += aDelta;
		}
	}

	void Instance::Render(const Camera& aCamera)
	{
		if (myShouldRender == true && myProxy.IsLoaded())
		{
			myProxy.GetEffect()->SetViewProjectionMatrix(aCamera.GetViewProjection());
			myProxy.GetEffect()->SetScaleVector(myScale);
			myProxy.GetEffect()->SetCameraPosition(aCamera.GetOrientation().GetPos());

			if (myProxy.IsAnimated() == true)
			{
				myProxy.GetEffect()->SetBones(myBones);
				RenderModelAnimated(myProxy.myModelAnimated, myOrientation, aCamera, myHierarchy);
			}
			else
			{
				myProxy.Render(myOrientation, aCamera.GetOrientation().GetPos());
			}
		}
	}

	void Instance::Render(const Camera& aCamera, InstancingHelper& aInstancingHelper)
	{

		if (myShouldRender == true && myProxy.IsLoaded())
		{
			if (myProxy.IsAnimated() == true)
			{
				if (aCamera.GetFrustum().Inside(myOrientation.GetPos(), myProxy.myModelAnimated->GetRadius()) == true)
				{
					myProxy.GetEffect()->SetViewProjectionMatrix(aCamera.GetViewProjection());
					myProxy.GetEffect()->SetScaleVector(myScale);
					myProxy.GetEffect()->SetCameraPosition(aCamera.GetOrientation().GetPos());

					myProxy.GetEffect()->SetBones(myBones);
					RenderModelAnimated(myProxy.myModelAnimated, myOrientation, aCamera, myHierarchy);
				}
			}
			else
			{
				if (aCamera.GetFrustum().Inside(myOrientation.GetPos(), myProxy.myModel->GetRadius()) == true)
				{
					AddModelToInstancingHelper(myProxy.myModel, aInstancingHelper);
				}
			}
		}
	}

	void Instance::AddModelToInstancingHelper(Model* aModel, InstancingHelper& aInstancingHelper)
	{
		if (aModel->IsNullObject() == false)
		{
			aInstancingHelper.AddModel(aModel, myOrientation, myScale, myOrientation.GetPos().y);
		}

		for (int i = 0; i < aModel->GetChildren().Size(); ++i)
		{
			AddModelToInstancingHelper(aModel->GetChildren()[i], aInstancingHelper);
		}
	}

	void Instance::RenderModelAnimated(ModelAnimated* aModel, const CU::Matrix44<float>& aParent
		, const Camera& aCamera, TransformationNodeInstance& aHierarchy)
	{
		if (aModel->myIsNULLObject == false)
		{
			aModel->Render(aHierarchy.GetTransformation() * aParent);
		}

		for (int i = 0; i < aHierarchy.GetChildren().Size(); ++i)
		{
			DL_ASSERT_EXP(aModel->myChildren[i] != nullptr, "Missmatch number of TransformationNodes and number of Models");

			RenderModelAnimated(aModel->myChildren[i], aHierarchy.GetTransformation() * aParent, aCamera, *aHierarchy.GetChildren()[i]);
		}
	}

	CU::Vector3<float> Instance::GetPosition() const
	{
		return myOrientation.GetPos();
	}

	void Instance::SetEffect(const std::string& aEffectFile)
	{
		if (myProxy.IsLoaded())
		{
			myProxy.SetEffect(EffectContainer::GetInstance()->GetEffect(aEffectFile));
		}
	}

	void Instance::SetScale(const CU::Vector3<float>& aScaleVector)
	{
		myScale = aScaleVector;

	}

	void Instance::UpdateDirectionalLights(
		const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData)
	{
		if (myProxy.IsLoaded())
		{
			myProxy.GetEffect()->UpdateDirectionalLights(someDirectionalLightData);
		}
	}

	void Instance::UpdatePointLights(
		const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData)
	{
		if (myProxy.IsLoaded())
		{
			myProxy.GetEffect()->UpdatePointLights(somePointLightData);
		}
	}

	void Instance::UpdateSpotLights(
		const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData)
	{
		if (myProxy.IsLoaded())
		{
			myProxy.GetEffect()->UpdateSpotLights(someSpotLightData);
		}
	}

	void Instance::SetAnimation(Animation* aAnimation)
	{
		myAnimation = aAnimation;
	}

	Animation* Instance::GetCurrentAnimation() const
	{
		return myAnimation;
	}

	bool Instance::IsAnimationDone()
	{
		if (myAnimation == nullptr) return true;

		if (myTotalTime > myAnimation->GetAnimationLenght()) return true;

		return false;
	}

	void Instance::ResetAnimationTime(float aTime)
	{
		myTotalTime = aTime;
	}

	ModelProxy& Instance::GetModel()
	{
		return myProxy;
	}

	void Instance::BuildHierarchy(TransformationNodeInstance& aHierarchy, ModelAnimated* aModel)
	{
		aHierarchy.SetTransformationNode(aModel->myTransformation);

		for (int i = 0; i < aModel->myChildTransforms.Size(); ++i)
		{
			SET_RUNTIME(false);
			TransformationNodeInstance* newNode = new TransformationNodeInstance();
			RESET_RUNTIME;
			newNode->SetTransformationNode(aModel->myChildTransforms[i]);
			aHierarchy.AddChildNode(newNode);

			BuildHierarchy(*newNode, aModel->myChildren[i]);
		}
	}
}