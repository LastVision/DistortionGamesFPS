#pragma once
#include <GrowingArray.h>
#include <Matrix.h>
#include <NetworkMessageTypes.h>
#include <SharedLevel.h>

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Scene;
	class Instance;
	class Room;
	class PointLight;
}

class EmitterManager;

class ClientLevel : public SharedLevel
{
public:
	ClientLevel();
	~ClientLevel();

	void Init() override;
	void SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint);

	void Update(const float aDeltaTime) override;
	void Render();
	bool connected;
	Prism::Scene* GetScene();

	void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress) override;

	void AddLight(Prism::PointLight* aLight);
	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond) override;
	void DebugMusic();

private:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity) override;
	void CreatePlayers();
	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;

	CU::GrowingArray<Entity*> myInstances;
	CU::GrowingArray<CU::Matrix44f> myInstanceOrientations;
	CU::GrowingArray<Prism::PointLight*> myPointLights;

	Entity* myPlayer;
	EmitterManager* myEmitterManager;
	bool myInitDone;

	CU::Vector3<float> myMinPoint;
	CU::Vector3<float> myMaxPoint;
};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
