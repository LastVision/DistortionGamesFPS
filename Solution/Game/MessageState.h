#pragma once
#include "GameState.h"

namespace Prism
{
	class SpriteProxy;
}
struct GameStateMessage;

class MessageState : public GameState
{
public:
	MessageState(const std::string& aTexturePath, const CU::Vector2<float>& aSize);
	~MessageState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void SetText(const std::string& aText);
	void SetEvent(GameStateMessage* anEvent);

private:

	Prism::SpriteProxy* myBackground;

	Prism::SpriteProxy* myBlackOverlay;

	CU::Vector2<float> mySpriteSize;

	std::string myTextMessage;
	CU::Vector2<float> myMessagePosition;
	GameStateMessage* myEvent;

	float myTotalTime;
	float myFinishedTextAlpha;
	bool myFinishedTextFadeIn;
};

inline void MessageState::SetText(const std::string& aText)
{
	myTextMessage = aText;
}

inline void MessageState::SetEvent(GameStateMessage* anEvent)
{
	myEvent = anEvent;
}
