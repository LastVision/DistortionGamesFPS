#pragma once
#include <Matrix.h>

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

	void Update(const float aDeltaTime);
	void Render();

private:
	Prism::Scene* myScene;

	Prism::Instance* myInstance;
	Player* myPlayer;

	CU::Matrix44f myInstanceOrientation;
	CU::Matrix44f myPlayerOrientation;
};

