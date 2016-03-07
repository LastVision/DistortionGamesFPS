#pragma once
#include <GrowingArray.h>
#include <Matrix.h>
#include <NetworkMessageTypes.h>
#include <SharedLevel.h>
class Player;

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Scene;
	class Instance;
	class Room;
}

class EmitterManager;

class ClientLevel : public SharedLevel
{
public:
	ClientLevel();
	~ClientLevel();

	void Update(const float aDeltaTime) override;
	void Render();
	bool connected;
	Prism::Scene* GetScene();
	void ReceiveMessage(const NetworkAddPlayerMessage& aMessage) override;
	void ReceiveMessage(const NetworkAddEnemyMessage& aMessage) override;


private:
	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;

	CU::GrowingArray<Entity*> myInstances;
	CU::GrowingArray<CU::Matrix44f> myInstanceOrientations;

	Player* myPlayer;
	EmitterManager* myEmitterManager;

};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
