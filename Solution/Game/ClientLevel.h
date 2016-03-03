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

class ClientLevel : public SharedLevel
{
public:
	ClientLevel();
	~ClientLevel();

	void Update(const float aDeltaTime) override;
	void Render();
	bool connected;
	Prism::Scene* GetScene();

private:
	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;

	Player* myPlayer;
	CU::GrowingArray<OtherClients> myClients;


};

inline Prism::Scene* ClientLevel::GetScene()
{
	return myScene;
}
