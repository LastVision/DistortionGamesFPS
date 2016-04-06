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

class CompleteGameState : public GameState, public Subscriber
{
public:
	CompleteGameState();
	~CompleteGameState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

private:
	GUI::GUIManager* myGUIManager;
	float myWaitTimer;
};