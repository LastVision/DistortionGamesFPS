#pragma once
#include "GameState.h"

namespace GUI
{
	class GUIManager;
	class Cursor;
}

namespace Prism
{
	class Text;
}

class ClickableState : public GameState
{
public:
	ClickableState(const float aTime, Prism::Text* aClickableText);
	~ClickableState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;
private:
	float myTimer;
	float myCurrentTime;

	Prism::Text* myClickableText;

	GUI::GUIManager* myGUIManager;
};

