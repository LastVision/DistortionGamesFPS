#pragma once
#include "GameState.h"

namespace Prism
{
	class SpriteProxy;
}

namespace GUI
{
	class Cursor;
	class GUIManager;
}

class LoadingState : public GameState
{
public:
	LoadingState();
	~LoadingState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

private:
	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	Prism::SpriteProxy* myRotatingThing;
	Prism::SpriteProxy* myRotatingThing2;
	float myRotatingThingScale;

	float myFinishedTextAlpha;
	bool myFinishedTextFadeIn;
	bool myIsDone;

};

