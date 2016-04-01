#pragma once
#include <GrowingArray.h>
#include <Matrix.h>
#include <NetworkMessageTypes.h>
#include <SharedLevel.h>

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Renderer;
	class Scene;
	class Instance;
	class Room;
	class PointLight;
	class SpotLight;
}

class EmitterManager;
class TextEventManager;

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
	
	Prism::Scene* GetScene();

	void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageShootGrenade& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageExplosion& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRayCastRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void AddLight(Prism::PointLight* aLight);
	void AddLight(Prism::SpotLight* aLight);
	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered) override;
	void DebugMusic();

private:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered) override;
	void CreatePlayers();
	void HandleOtherClientRayCastPistol(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);
	void HandleOtherClientRayCastShotgun(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);

	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> myOtherClientRaycastHandlerPistol;
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> myOtherClientRaycastHandlerShotgun;

	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;
	Prism::Renderer* myFullscreenRenderer;

	CU::GrowingArray<Entity*> myInstances;
	CU::GrowingArray<CU::Matrix44f> myInstanceOrientations;
	CU::GrowingArray<Prism::PointLight*> myPointLights;
	CU::GrowingArray<Prism::SpotLight*> mySpotLights;

	Entity* myPlayer;
	EmitterManager* myEmitterManager;
	bool myInitDone;

	CU::Vector3<float> myMinPoint;
	CU::Vector3<float> myMaxPoint;
	std::string myName;

	TextEventManager* myTextManager;

	float myForceStrengthPistol;
	float myForceStrengthShotgun;
};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
