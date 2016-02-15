#pragma once
#include "Defines.h"
#include <GrowingArray.h>
#include "LightStructs.h"
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Instance;
	class PointLight;
	class SpotLight;
	class SpotLightShadow;
	class InstancingHelper;
	class Texture;
#ifdef SCENE_USE_OCTREE
	class Octree;
#endif

	class Scene
	{
	public:
		Scene(const Camera& aCamera);
		~Scene();

		void Render(bool aRenderNavMeshLines);
		void Render(bool aRenderNavMeshLines, Texture* aFogOfWarTexture, SpotLightShadow* aShadowSpotLight);
		void OnResize(int aWidth, int aHeigth);

		void AddInstance(Instance* aInstance, bool aIsSelectionRing = false);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);

		void RemoveInstance(Instance* aInstance, bool aIsSelectionRing = false);

		void SetCamera(const Camera& aCamera);
		void SetViewCamera(const Camera& aCamera);
		const Camera* GetCamera() const;

		void CalcShouldRender(const Prism::Camera& aCamera);
		
	private:
		void operator=(Scene&) = delete;
#ifdef SCENE_USE_OCTREE
		Octree* myOctree;
#endif
		CU::GrowingArray<Instance*> myInstances;
		CU::GrowingArray<Instance*> myDynamicInstances;
		CU::GrowingArray<Instance*> mySelectionCircles;
		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;
		InstancingHelper* myInstancingHelper;

		const Camera* myCamera;
		const Camera* myViewCamera;

		CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS> myDirectionalLightData;
		CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS> myPointLightData;
		CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS> mySpotLightData;
		float myRenderRadius;
	};



	inline const Camera* Scene::GetCamera() const
	{
		return myCamera;
	}
}