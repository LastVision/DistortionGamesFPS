#pragma once
#include "GameState.h"

namespace Prism 
{
	class SpriteProxy;
}

class SplashState : public GameState
{
public:
	SplashState(const std::string& aTexturePath, bool aStartSound, bool aVictoryScreen = false);
	~SplashState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

protected:
	bool myFadeOut;
	bool myDisplay;
	float myLogoAlpha;

	float myFadeInTime;
	float myFadeOutTime;
	float myDisplayTime;
	float myCurrentTime;
	Prism::Camera* myCamera;

	Prism::SpriteProxy* myLogo;
	Prism::SpriteProxy* myBackground;

	bool myVictoryScreen;

	bool myStartSound;
};

