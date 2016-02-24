#pragma once
#include <GrowingArray.h>
#include <Matrix.h>
#include <NetworkMessageTypes.h>
class Player;

namespace Prism
{
	class Camera;
	class Scene;
	class Instance;
}

class Level
{
public:
	Level();
	~Level();

	void AddEntity(Entity* aEntity);

	void Update(const float aDeltaTime);
	void Render();
	bool connected;
	Prism::Scene* GetScene();

private:
	Prism::Scene* myScene;

	Player* myPlayer;
	CU::GrowingArray<Entity*> myEntities;
	CU::GrowingArray<OtherClients> myClients;


};

inline Prism::Scene* Level::GetScene()
{
	return myScene;
}
