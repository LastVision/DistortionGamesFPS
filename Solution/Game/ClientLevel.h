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

	void Update(const float aDeltaTime) override;
	void Render();
	bool connected;
	Prism::Scene* GetScene();
	void ReceiveMessage(const PostMasterNetAddPlayerMessage& aMessage) override;
	void ReceiveMessage(const PostMasterNetRemovePlayerMessage& aMessage) override;
	void ReceiveMessage(const PostMasterNetAddEnemyMessage& aMessage) override;
	void ReceiveMessage(const PostMasterNetOnDeathMessage& aMessage) override;

	void AddLight(Prism::PointLight* aLight);

	void DebugMusic();

private:
	void CreatePlayers();
	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;

	CU::GrowingArray<Entity*> myInstances;
	CU::GrowingArray<CU::Matrix44f> myInstanceOrientations;
	CU::GrowingArray<Prism::PointLight*> myPointLights;

	Entity* myPlayer;
	EmitterManager* myEmitterManager;
	bool myInitDone;
};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
