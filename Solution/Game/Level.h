#pragma once
#include <Subscriber.h>

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Renderer;
	class Scene;
	class SpotLightShadow;
	class Terrain;
	class FogOfWarHelper;
}

namespace GUI
{
	class Cursor;
}

class AIDirector;
class Entity;
class EmitterManager;
class NeutralDirector;
class PlayerDirector;
class Tutorial;

class Level : public Subscriber
{
	friend class LevelFactory;
public:
	Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain, GUI::Cursor* aCursor, eDifficulty aDifficulty);
	~Level();

	void LoadTutorial(const Prism::Camera& aCamera, const std::string& aTutorialPath);

	bool Update(float aDeltaTime, Prism::Camera& aCamera);
	void Render(Prism::Camera& aCamera);

	void OnResize(int aWidth, int aHeigth);

	void ReceiveMessage(const ToggleRenderLinesMessage& aMessage) override;
	void ReceiveMessage(const ToggleFogOfWarMessage& aMessage) override;

	void SpawnUnit(eUnitType aUnitType);

	bool HasPlayerWon() const;
	bool HasAIWon() const;

	CU::Vector3<float> GetCameraMoveVector() const;

	PlayerDirector* GetPlayer();

private:
	void DoFogCulling();

	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::SpotLightShadow* myShadowLight;
	Prism::Scene* myScene;
	Prism::Renderer* myRenderer;
	Tutorial* myTutorial;

	EmitterManager* myEmitterManager;

	CU::GrowingArray<Entity*> myEntities;
	PlayerDirector* myPlayer;
	AIDirector* myAI;
	NeutralDirector* myNeutralDirector;

	Prism::FogOfWarHelper* myFogOfWarHelper;

	int myMaxVictoryPoint;

	bool myRenderNavMeshLines;
	bool myShowFogOfWar;

	bool myHasToldPlayerAboutWinningIn50;
	bool myHasToldPlayerAboutLosingIn50;
	bool myHasToldPlayerAboutWinningIn100;
	bool myHasToldPlayerAboutLosingIn100;
	bool myHasToldPlayerAboutWinningIn200;
	bool myHasToldPlayerAboutLosingIn200;
	bool myHasToldPlayerAboutWinningIn300;
	bool myHasToldPlayerAboutLosingIn300;

	bool myIsFirstFrame;
	bool myIsSecondFrame;
};

inline PlayerDirector* Level::GetPlayer()
{
	return myPlayer;
}