#pragma once
#include <Defines.h>
#include <GrowingArray.h>
#include "LightStructs.h"
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Instance;
	class PointLight;
	class Room;
	class RoomManager;
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
		void UpdateLights();

		void OnResize(int aWidth, int aHeigth);

		void AddRoom(Room* aRoom);
		void AddInstance(Instance* aInstance, bool aAlwaysRender = false);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);

		void RemoveInstance(Instance* aInstance);

		void SetCamera(const Camera& aCamera);
		const Camera* GetCamera() const;
		RoomManager* GetRoomManager() const;

		const CU::GrowingArray<PointLight*>& GetPointLights() const;

	private:
		void operator=(Scene&) = delete;

		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;
		InstancingHelper* myInstancingHelper;

		const Camera* myCamera;
		RoomManager* myRoomManager;

		CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS> myDirectionalLightData;
		CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS> myPointLightData;
		CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS> mySpotLightData;
		float myRenderRadius;
	};



	inline const Camera* Scene::GetCamera() const
	{
		return myCamera;
	}

	inline const CU::GrowingArray<PointLight*>& Scene::GetPointLights() const
	{
		return myPointLights;
	}

	inline RoomManager* Scene::GetRoomManager() const
	{
		return myRoomManager;
	}
}