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
	class SpotLightTextureProjection;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Render();
		void RenderArmAndWeapon();
		void RenderArmAndWeaponOnlyDepth();
		void RenderWithoutRoomManager();
		void UpdateLights();

		void AddRoom(Room* aRoom);
		void AddInstance(Instance* aInstance, eObjectRoomType aRoomType);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);
		void AddLight(SpotLightTextureProjection* aLight);

		void RemoveInstance(Instance* aInstance);

		void SetCamera(Camera& aCamera);
		Camera* GetCamera() const;
		RoomManager* GetRoomManager() const;

		const CU::GrowingArray<PointLight*>& GetPointLights(bool aUseRoomManager) const;
		const CU::GrowingArray<SpotLight*>& GetSpotLights(bool aUseRoomManager) const;
		const CU::GrowingArray<SpotLightTextureProjection*>& GetSpotLightsTextureProjection(bool aUseRoomManager) const;

		void SetArmInstance(Instance* aInstance);
		void SetWeaponInstance(Instance* aInstance);

	private:
		void operator=(Scene&) = delete;

		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<PointLight*> myAmbientPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;
		CU::GrowingArray<SpotLightTextureProjection*> mySpotLightsTextureProjection;
		InstancingHelper* myInstancingHelper;

		Camera* myCamera;
		RoomManager* myRoomManager;
		Instance* myArmInstance;
		Instance* myWeaponInstance;
	};



	inline Camera* Scene::GetCamera() const
	{
		return myCamera;
	}


	inline RoomManager* Scene::GetRoomManager() const
	{
		return myRoomManager;
	}
}