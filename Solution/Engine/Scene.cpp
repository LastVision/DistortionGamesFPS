#include "stdafx.h"

#include "Camera.h"
#include <Defines.h>
#include "DirectionalLight.h"
#include "EngineEnums.h"
#include "Frustum.h"
#include "Instance.h"
#include "PointLight.h"
#include "RoomManager.h"
#include "Scene.h"
#include "SpotLight.h"
#include "InstancingHelper.h"

namespace Prism
{
	Scene::Scene()
		: myCamera(nullptr)
		, myRenderRadius(-10.f)
	{
		myDirectionalLights.Init(4);
		myPointLights.Init(128);
		mySpotLights.Init(4);

		memset(&myDirectionalLightData[0], 0, sizeof(DirectionalLightData) * NUMBER_OF_DIRECTIONAL_LIGHTS);
		memset(&myPointLightData[0], 0, sizeof(PointLightData) * NUMBER_OF_POINT_LIGHTS);
		memset(&mySpotLightData[0], 0, sizeof(SpotLightData) * NUMBER_OF_SPOT_LIGHTS);

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

		myInstancingHelper->Render(myDirectionalLightData);
	}

	void Scene::RenderWithoutRoomManager()
	{
		UpdateLights();

		const CU::GrowingArray<Instance*>& instances = myRoomManager->GetAllInstances();

		for (int i = 0; i < instances.Size(); ++i)
		{
			instances[i]->Render(*myCamera, *myInstancingHelper);
		}

		myInstancingHelper->Render(myDirectionalLightData);
	}

	void Scene::OnResize(int aWidth, int aHeigth)
	{
		float ratio = static_cast<float>(aWidth) / static_cast<float>(aHeigth);

		myRenderRadius = -10.f;

		float epsilon = 0.1f;
		if (ratio + epsilon >= 16.f / 9.f)
		{
			myRenderRadius = -17.f;
		}
		else if (ratio + epsilon >= 16.f / 10.f)
		{
			myRenderRadius = -15.f;
		}
	}

	void Scene::AddRoom(Room* aRoom)
	{
		myRoomManager->Add(aRoom);
	}

	void Scene::AddInstance(Instance* aInstance, eObjectRoomType aRoomType)
	{
		DL_ASSERT_EXP(myRoomManager != nullptr, "No Room manager");
		myRoomManager->Add(aInstance, aRoomType);
	}

	void Scene::AddLight(DirectionalLight* aLight)
	{
		myDirectionalLights.Add(aLight);
	}

	void Scene::AddLight(PointLight* aLight)
	{
		myPointLights.Add(aLight);
		myRoomManager->Add(aLight);
	}

	void Scene::AddLight(SpotLight* aLight)
	{
		mySpotLights.Add(aLight);
	}

	void Scene::SetCamera(const Camera& aCamera)
	{
		myCamera = &aCamera;
		myInstancingHelper->SetCamera(myCamera);
	}

	void Scene::UpdateLights()
	{
		for (int i = 0; i < myDirectionalLights.Size(); ++i)
		{
			myDirectionalLights[i]->Update();

			myDirectionalLightData[i].myDirection = myDirectionalLights[i]->GetCurrentDir();
			myDirectionalLightData[i].myColor = myDirectionalLights[i]->GetColor();
		}

		for (int i = 0; i < myPointLights.Size(); ++i)
		{
			myPointLights[i]->Update();
		}
		
		for (int i = 0; i < mySpotLights.Size(); ++i)
		{
			mySpotLights[i]->Update();

			//mySpotLightData[i].myPosition = mySpotLights[i]->GetPosition();
			//mySpotLightData[i].myDirection = mySpotLights[i]->GetDir();
			//mySpotLightData[i].myColor = mySpotLights[i]->GetColor();
			//mySpotLightData[i].myRange = mySpotLights[i]->GetRange();
			//mySpotLightData[i].myCone = mySpotLights[i]->GetCone();
		}
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
		//return myPointLights;
		//return myActivePointLights;
		if (aUseRoomManager == true)
		{
			return myRoomManager->GetActivePointLights();
		}

		return myPointLights;
	}

	const CU::GrowingArray<SpotLight*>& Scene::GetSpotLights(bool aUseRoomManager) const
	{
		if (aUseRoomManager == true)
		{
			//return myRoomManager->GetActiveSpotLights();
			return mySpotLights;
		}

		return mySpotLights;
	}
}