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

	void Init(const std::string& aMissionXMLPath = "") override;
	void SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint);
	void SetName(const std::string& aName);

	void Update(const float aDeltaTime) override;
	void Render();
	bool connected;
	Prism::Scene* GetScene();

	void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageShootGrenade& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageExplosion& aMessage, const sockaddr_in& aSenderAddress) override;
	void AddLight(Prism::PointLight* aLight);
	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered) override;
	void DebugMusic();

private:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered) override;
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
	std::string myName;
};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
