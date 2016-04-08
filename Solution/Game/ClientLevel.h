#pragma once
#include <GrowingArray.h>
#include <Matrix.h>
#include <NetworkMessageTypes.h>
#include <SharedLevel.h>
#include "StateEnums.h"

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

namespace GUI
{
	class Cursor;
	class GUIManager;
}

class EmitterManager;
class TextEventManager;

class ClientLevel : public SharedLevel
{
public:
	ClientLevel(GUI::Cursor* aCursor, eStateStatus& aStateStatus);
	~ClientLevel();

	void Init(const std::string& aWeaponSettingsPath) override;
	void SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint);
	void SetName(const std::string& aName);

	void Update(const float aDeltaTime, bool aLoadingScreen) override;
	void Render();
	
	Prism::Scene* GetScene();

	void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageShootGrenade& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageExplosion& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRayCastRequest& aMessage, const sockaddr_in& aSenderAddress) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

	void AddLight(Prism::PointLight* aLight);
	void AddLight(Prism::SpotLight* aLight);
	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered) override;
	void AddWorldText(const std::string& aText, const CU::Vector3<float>& aPosition, float aRotationAroundY, const CU::Vector4<float>& aColor);

	void OnResize(float aWidth, float aHeight);
	void ToggleEscapeMenu();

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

	struct WorldText
	{
		Prism::TextProxy* myProxy;
		std::string myText;
	};
	CU::GrowingArray<WorldText> myWorldTexts;

	bool myEscapeMenuActive;
	GUI::GUIManager* myEscapeMenu;

	Prism::TextProxy* mySFXText;
	Prism::TextProxy* myMusicText;
	Prism::TextProxy* myVoiceText;

	int	myMusicVolume;
	int	mySfxVolume;
	int	myVoiceVolume;

	eStateStatus& myStateStatus;
};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
