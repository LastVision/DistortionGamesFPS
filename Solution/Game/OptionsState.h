#pragma once
#include "Subscriber.h"
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

namespace Prism
{
	class Text;
}

class OptionsState : public GameState, public Subscriber
{
public:
	OptionsState();
	~OptionsState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void ReceiveMessage(const OnClickMessage& aMessage);

private:
	GUI::GUIManager* myGUIManager;
	Prism::Text* myMusicText;
	Prism::Text* mySfxText;
	Prism::Text* myVoiceText;
	Prism::Text* myShadowText;

	int myMusicVolume;
	int mySfxVolume;
	int myVoiceVolume;
};