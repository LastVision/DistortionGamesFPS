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
		Scene();
		~Scene();

		void Render();

		void OnResize(int aWidth, int aHeigth);

		void AddInstance(Instance* aInstance);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);

		void RemoveInstance(Instance* aInstance);

		void SetCamera(const Camera& aCamera);
		const Camera* GetCamera() const;

	private:
		void operator=(Scene&) = delete;
		void UpdateLights();

		CU::GrowingArray<Instance*> myInstances;
		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;
		InstancingHelper* myInstancingHelper;

		const Camera* myCamera;

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