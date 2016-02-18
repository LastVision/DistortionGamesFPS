#pragma once
#include <GrowingArray.h>
#include <Matrix.h>

class PlayerComponent;

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

	Prism::Scene* GetScene();

private:
	Prism::Scene* myScene;

	Prism::Instance* myInstance;
	PlayerComponent* myPlayer;

	CU::Matrix44f myInstanceOrientation;
	CU::Matrix44f myPlayerOrientation;

	CU::GrowingArray<Entity*> myEntities;
};

inline Prism::Scene* Level::GetScene()
{
	return myScene;
}
