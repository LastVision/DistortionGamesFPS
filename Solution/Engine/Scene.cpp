#include "stdafx.h"

#include <AABB.h>
#include "Camera.h"
#include "Defines.h"
#include "DirectionalLight.h"
#include "EngineEnums.h"
#include "Frustum.h"
#include "Ice.h"
#include "Instance.h"
#include "PointLight.h"
#include "Scene.h"
#include "SpotLight.h"
#include "Terrain.h"
#include <XMLReader.h>
#include "InstancingHelper.h"

#include <InputWrapper.h>

#ifdef SCENE_USE_OCTREE
#include "Octree.h"
#endif

Prism::Scene::Scene(const Camera& aCamera, Terrain& aTerrain)
	: myCamera(&aCamera)
	, myViewCamera(nullptr)
	, myTerrain(aTerrain)
	, myRenderRadius(-10.f)
#ifdef SCENE_USE_OCTREE
	, myOctree(new Octree(6))
#endif
{
	myInstances.Init(4096);
	myDynamicInstances.Init(64);
	myDirectionalLights.Init(4);
	myPointLights.Init(4);
	mySpotLights.Init(4);
	mySelectionCircles.Init(64);

	memset(&myDirectionalLightData[0], 0, sizeof(DirectionalLightData) * NUMBER_OF_DIRECTIONAL_LIGHTS);
	memset(&myPointLightData[0], 0, sizeof(PointLightData) * NUMBER_OF_POINT_LIGHTS);
	memset(&mySpotLightData[0], 0, sizeof(SpotLightData) * NUMBER_OF_SPOT_LIGHTS);

	myInstancingHelper = new InstancingHelper();
	myInstancingHelper->SetCamera(myCamera);
}

Prism::Scene::~Scene()
{

#ifdef SCENE_USE_OCTREE
	delete myOctree;
	myOctree = nullptr;

#else
	myInstances.RemoveAll();
#endif

	SAFE_DELETE(myInstancingHelper);
}

void Prism::Scene::Render(bool)
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

		myPointLightData[i].myColor = myPointLights[i]->GetColor();
		myPointLightData[i].myPosition = myPointLights[i]->GetPosition();
		myPointLightData[i].myRange = myPointLights[i]->GetRange();
	}

	for (int i = 0; i < mySpotLights.Size(); ++i)
	{
		mySpotLights[i]->Update();

		mySpotLightData[i].myPosition = mySpotLights[i]->GetPosition();
		mySpotLightData[i].myDirection = mySpotLights[i]->GetDir();
		mySpotLightData[i].myColor = mySpotLights[i]->GetColor();
		mySpotLightData[i].myRange = mySpotLights[i]->GetRange();
		mySpotLightData[i].myCone = mySpotLights[i]->GetCone();
	}

	//myTerrain.GetEffect()->UpdateDirectionalLights(myDirectionalLightData);
	//myTerrain.GetIce()->GetEffect()->UpdateDirectionalLights(myDirectionalLightData);
	//myTerrain.UpdatePointLights(myPointLightData);
	//myTerrain.UpdateSpotLights(mySpotLightData);
	//myTerrain.Render(*myCamera, aRenderNavMeshLines, true);

#ifdef SCENE_USE_OCTREE
	myOctree->Update();
	myInstances.RemoveAll();
	myOctree->GetOccupantsInAABB(myCamera.GetFrustum(), myInstances);
	DEBUG_PRINT(myInstances.Size());

	for (int i = 0; i < myDynamicInstances.Size(); ++i)
	{
		myDynamicInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
		myDynamicInstances[i]->UpdatePointLights(myPointLightData);
		myDynamicInstances[i]->UpdateSpotLights(mySpotLightData);
		myDynamicInstances[i]->Render(myCamera);
	}
#ifdef SHOW_OCTREE_DEBUG
	Engine::GetInstance()->PrintText(myInstances.Size(), { 600.f, -600.f });
#endif
#endif
	/*for (int i = 0; i < myInstances.Size(); ++i)
	{
		myInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
		myInstances[i]->UpdatePointLights(myPointLightData);
		myInstances[i]->UpdateSpotLights(mySpotLightData);
		myInstances[i]->Render(myCamera);
	}*/

	for (int i = 0; i < myInstances.Size(); ++i)
	{
		Instance* current = myInstances[i];
		if (current->GetCastShadow() == true
			&& (current->GetAlwaysRender() == true || (current->GetShouldRender() == true && current->GetRenderThroughCulling() == true)))
		{
			current->Render(*myCamera, *myInstancingHelper, true);
		}
	}

	myInstancingHelper->Render(myDirectionalLightData, true);
}

void Prism::Scene::Render(bool aRenderNavMeshLines, Texture* aFogOfWarTexture, SpotLightShadow* aShadowSpotLight)
{
	Prism::EffectContainer::GetInstance()->SetShadowDepth(aShadowSpotLight);

	for (int i = 0; i < myDirectionalLights.Size(); ++i)
	{
		myDirectionalLights[i]->Update();

		myDirectionalLightData[i].myDirection = myDirectionalLights[i]->GetCurrentDir();
		myDirectionalLightData[i].myColor = myDirectionalLights[i]->GetColor();
	}

	for (int i = 0; i < myPointLights.Size(); ++i)
	{
		myPointLights[i]->Update();

		myPointLightData[i].myColor = myPointLights[i]->GetColor();
		myPointLightData[i].myPosition = myPointLights[i]->GetPosition();
		myPointLightData[i].myRange = myPointLights[i]->GetRange();
	}

	for (int i = 0; i < mySpotLights.Size(); ++i)
	{
		mySpotLights[i]->Update();

		mySpotLightData[i].myPosition = mySpotLights[i]->GetPosition();
		mySpotLightData[i].myDirection = mySpotLights[i]->GetDir();
		mySpotLightData[i].myColor = mySpotLights[i]->GetColor();
		mySpotLightData[i].myRange = mySpotLights[i]->GetRange();
		mySpotLightData[i].myCone = mySpotLights[i]->GetCone();
	}

	myTerrain.GetEffect()->UpdateDirectionalLights(myDirectionalLightData);
	myTerrain.GetIce()->GetEffect()->UpdateDirectionalLights(myDirectionalLightData);

	myTerrain.GetEffect()->SetFogOfWarTexture(aFogOfWarTexture);
	myTerrain.GetIce()->GetEffect()->SetFogOfWarTexture(aFogOfWarTexture);
	//myTerrain.UpdatePointLights(myPointLightData);
	myTerrain.GetEffect()->UpdateSpotLights(mySpotLightData);
	myTerrain.GetIce()->GetEffect()->UpdateSpotLights(mySpotLightData);
	myTerrain.Render(*myCamera, aRenderNavMeshLines, false);

	myTerrain.GetEffect()->SetFogOfWarTexture(nullptr);
	myTerrain.GetIce()->GetEffect()->SetFogOfWarTexture(nullptr);

#ifdef SCENE_USE_OCTREE
	myOctree->Update();
	myInstances.RemoveAll();
	myOctree->GetOccupantsInAABB(myCamera.GetFrustum(), myInstances);
	DEBUG_PRINT(myInstances.Size());

	for (int i = 0; i < myDynamicInstances.Size(); ++i)
	{
		myDynamicInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
		myDynamicInstances[i]->UpdatePointLights(myPointLightData);
		myDynamicInstances[i]->UpdateSpotLights(mySpotLightData);
		myDynamicInstances[i]->Render(myCamera);
	}
#ifdef SHOW_OCTREE_DEBUG
	Engine::GetInstance()->PrintText(myInstances.Size(), { 600.f, -600.f });
#endif
#endif
	/*for (int i = 0; i < myInstances.Size(); ++i)
	{
	myInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
	myInstances[i]->UpdatePointLights(myPointLightData);
	myInstances[i]->UpdateSpotLights(mySpotLightData);
	myInstances[i]->Render(myCamera);
	}*/

	static int maxVisibileInstances = 0;
	int visibleInstances = 0;
	for (int i = 0; i < myInstances.Size(); ++i)
	{
		Instance* current = myInstances[i];
		if (current->GetAlwaysRender() == true || (current->GetShouldRender() == true && current->GetRenderThroughCulling() == true))
		{
			current->UpdateDirectionalLights(myDirectionalLightData);
			current->UpdateSpotLights(mySpotLightData);
			current->Render(*myCamera, *myInstancingHelper, false);

			++visibleInstances;
		}
	}

	if (visibleInstances > maxVisibileInstances)
	{
		maxVisibileInstances = visibleInstances;
	}

	DEBUG_PRINT(visibleInstances);
	DEBUG_PRINT(maxVisibileInstances);

	for (int i = 0; i < mySelectionCircles.Size(); ++i)
	{
		mySelectionCircles[i]->Render(*myCamera, *myInstancingHelper, false);
	}

	myInstancingHelper->Render(myDirectionalLightData, false);


	for (int i = 0; i < myInstances.Size(); ++i)
	{
		myInstances[i]->SetRenderThroughCulling(true);
	}
}

void Prism::Scene::OnResize(int aWidth, int aHeigth)
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

void Prism::Scene::AddInstance(Instance* aInstance, bool aIsSelectionRing)
{
	if(aIsSelectionRing == true)
	{
		mySelectionCircles.Add(aInstance);
	}
	else
	{
#ifdef SCENE_USE_OCTREE
		if (aInstance->GetOctreeType() == eOctreeType::DYNAMIC)
		{
			myDynamicInstances.Add(aInstance);
		}
		else
		{
			myOctree->Add(aInstance);
		}
#else
		myInstances.Add(aInstance);
#endif
	}
}

void Prism::Scene::AddLight(DirectionalLight* aLight)
{
	myDirectionalLights.Add(aLight);
}

void Prism::Scene::AddLight(PointLight* aLight)
{
	myPointLights.Add(aLight);
}

void Prism::Scene::AddLight(SpotLight* aLight)
{
	mySpotLights.Add(aLight);
}

void Prism::Scene::SetCamera(const Camera& aCamera)
{
	myCamera = &aCamera;
	myInstancingHelper->SetCamera(myCamera);
}

void Prism::Scene::SetViewCamera(const Camera& aCamera)
{
	myViewCamera = &aCamera;
}

void Prism::Scene::CalcShouldRender(const Prism::Camera& aCamera)
{
	DEBUG_PRINT(myRenderRadius);

	for (int i = 0; i < myInstances.Size(); ++i)
	{
		if (myInstances[i]->GetShouldRender() == true &&
			aCamera.GetFrustum().Inside(myInstances[i]->GetPosition(), myRenderRadius) == false)
		{
			myInstances[i]->SetRenderThroughCulling(false);
		}
	}
}

void Prism::Scene::RemoveInstance(Instance* aInstance, bool aIsSelectionRing)
{
	if (aIsSelectionRing == true)
	{
		int index = mySelectionCircles.Find(aInstance);
		if (index != mySelectionCircles.FoundNone)
		{
			mySelectionCircles.RemoveCyclicAtIndex(index);
		}
	}
	else
	{
#ifdef SCENE_USE_OCTREE
		if (aInstance->GetOctreeType() == eOctreeType::DYNAMIC)
		{
			myDynamicInstances.RemoveCyclic(aInstance);
		}
		else
		{
			myOctree->Remove(aInstance);
		}
#else
		int index = myInstances.Find(aInstance);
		if (index != myInstances.FoundNone)
		{
			myInstances.RemoveCyclicAtIndex(index);
		}
#endif
	}
}