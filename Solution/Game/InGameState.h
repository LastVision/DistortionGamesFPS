#pragma once
#include "GameState.h"
#include <Matrix.h>
#include <GrowingArray.h>
#include <Subscriber.h>

class Level;
class LevelFactory;
class MessageState;


namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class Sprite;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class InGameState : public GameState, public Subscriber
{
public:
	InGameState();
	~InGameState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

private:
	GUI::GUIManager* myGUIManager;
	Level* myLevel;
	LevelFactory* myLevelFactory;
};