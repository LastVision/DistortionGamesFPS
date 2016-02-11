#pragma once
#include "GameState.h"
#include <queue>

namespace Prism
{
	class SpriteProxy;
}

struct CinematicScene
{
	Prism::SpriteProxy* myBackground;
	CU::Vector2<float> myBackgroundPosition;
	Prism::SpriteProxy* myFirstImage;
	CU::Vector2<float> myFirstImagePosition;
	float myFirstImagePanningSpeed;
	Prism::SpriteProxy* mySecondImage;
	CU::Vector2<float> mySecondImagePosition;
	float mySecondImagePanningSpeed;

	float mySceneTime;
};

class CinematicState : public GameState
{
public:
	CinematicState(const std::string& aScenePath);
	~CinematicState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

private:
	void ReadScenes(const std::string& aScenePath);
	std::queue<CinematicScene> myScenes;
	float myTimer;
};

