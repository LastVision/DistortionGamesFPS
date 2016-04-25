#include "stdafx.h"


#include "Camera.h"
#include <Defines.h>
#include <d3dx11effect.h>
#include "DirectionalLight.h"
#include "EngineEnums.h"
#include "Frustum.h"
#include "Instance.h"
#include "PointLight.h"
#include "RoomManager.h"
#include "Scene.h"
#include "SpotLight.h"
#include "InstancingHelper.h"
#include "Texture.h"
#include "Engine.h"

namespace Prism
{
	Scene::Scene()
		: myCamera(nullptr)
		, myArmInstance(nullptr)
		, myWeaponInstance(nullptr)
	{
		myDirectionalLights.Init(4);
		myPointLights.Init(128);
		myAmbientPointLights.Init(128);
		mySpotLights.Init(4);
		mySpotLightsTextureProjection.Init(4);

		myInstancingHelper = new InstancingHelper();
		myInstancingHelper->SetCamera(myCamera);
		myRoomManager = new RoomManager();
	}

	Scene::~Scene()
	{
		SAFE_DELETE(myInstancingHelper);
		SAFE_DELETE(myRoomManager);
	}

	void Scene::Render()
	{
		UpdateLights();

		const CU::GrowingArray<Instance*>& instances = myRoomManager->GetActiveInstances(*myCamera);

		for (int i = 0; i < instances.Size(); ++i)
		{
			instances[i]->Render(*myCamera, *myInstancingHelper);
		}

		myInstancingHelper->Render();
	}

	void Scene::RenderArmAndWeapon()
	{
		//if (GC::ShouldRenderGUI == true)
		//{
		//	if (myArmInstance != nullptr)
		//	{
		//		myArmInstance->Render(*myCamera);
		//	}
		//
		//	if (myWeaponInstance != nullptr)
		//	{
		//		myWeaponInstance->Render(*myCamera);
		//	}
		//}
	}

	void Scene::RenderArmAndWeaponOnlyDepth()
	{
		//if (GC::ShouldRenderGUI == true)
		//{
		//	if (myArmInstance != nullptr)
		//	{
		//		myArmInstance->Render(*myCamera, true);
		//	}
		//
		//	if (myWeaponInstance != nullptr)
		//	{
		//		myWeaponInstance->Render(*myCamera, true);
		//	}
		//}
	}

	void Scene::RenderWithoutRoomManager()
	{
		UpdateLights();

		const CU::GrowingArray<Instance*>& instances = myRoomManager->GetAllInstances();

		for (int i = 0; i < instances.Size(); ++i)
		{
			instances[i]->Render(*myCamera, *myInstancingHelper);
		}

		myInstancingHelper->Render();

		if (GC::ShouldRenderGUI == true)
		{
			if (myArmInstance != nullptr)
			{
				myArmInstance->Render(*myCamera);
			}

			if (myWeaponInstance != nullptr)
			{
				myWeaponInstance->Render(*myCamera);
			}
		}
	}

	void Scene::AddRoom(Room* aRoom)
	{
		myRoomManager->Add(aRoom);
	}

	void Scene::AddInstance(Instance* aInstance, eObjectRoomType aRoomType)
	{
		DL_ASSERT_EXP(myRoomManager != nullptr, "No Room manager");
		//myRoomManager->Add(aInstance, aRoomType);
		myRoomManager->Add(aInstance, eObjectRoomType::ALWAYS_RENDER);
	}

	void Scene::AddLight(DirectionalLight* aLight)
	{
		myDirectionalLights.Add(aLight);
	}

	void Scene::AddLight(PointLight* aLight)
	{
		if (aLight->GetAmbientOnly() == false)
		{
			myPointLights.Add(aLight);
			myRoomManager->Add(aLight);
		}
		else
		{
			myAmbientPointLights.Add(aLight);
		}
	}

	void Scene::AddLight(SpotLight* aLight)
	{
		//mySpotLights.Add(aLight);
		myRoomManager->Add(aLight);
	}

	void Scene::AddLight(SpotLightTextureProjection* aLight)
	{
		myRoomManager->Add(aLight);
	}

	void Scene::SetCamera(Camera& aCamera)
	{
		myCamera = &aCamera;
		myInstancingHelper->SetCamera(myCamera);
	}

	void Scene::UpdateLights()
	{
		//for (int i = 0; i < myDirectionalLights.Size(); ++i)
		//{
		//	myDirectionalLights[i]->Update();
		//}
		//
		//for (int i = 0; i < myAmbientPointLights.Size(); ++i)
		//{
		//	myAmbientPointLights[i]->Update();
		//}
		//
		//for (int i = 0; i < mySpotLights.Size(); ++i)
		//{
		//	mySpotLights[i]->Update();
		//}
	}

	void Scene::RemoveInstance(Instance* aInstance)
	{
		if (myRoomManager != nullptr)
		{
			myRoomManager->Remove(aInstance);
		}
	}

	const CU::GrowingArray<PointLight*>& Scene::GetPointLights(bool aUseRoomManager) const
	{
		return myPointLights;
		//return myActivePointLights;
		if (aUseRoomManager == true)
		{
			return myRoomManager->GetActivePointLights();
		}

		return myAmbientPointLights;
	}

	const CU::GrowingArray<SpotLight*>& Scene::GetSpotLights(bool aUseRoomManager) const
	{
		if (aUseRoomManager == true)
		{
			return myRoomManager->GetActiveSpotLights();
		}

		return mySpotLights;
	}

	const CU::GrowingArray<SpotLightTextureProjection*>& Scene::GetSpotLightsTextureProjection(bool aUseRoomManager) const
	{
		if (aUseRoomManager == true)
		{
			return myRoomManager->GetActiveSpotLightsTextureProjection();
		}

		return mySpotLightsTextureProjection;
	}

	void Scene::SetArmInstance(Instance* aInstance)
	{
		myArmInstance = aInstance;
	}

	void Scene::SetWeaponInstance(Instance* aInstance)
	{
		myWeaponInstance = aInstance;
	}
}