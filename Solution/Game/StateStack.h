#pragma once
#include <GrowingArray.h>

namespace CU
{
	class InputWrapper;
}

namespace GUI
{
	class Cursor;
}

class GameState;
class StateStackProxy;

class StateStack
{
public:
	StateStack();
	~StateStack();

	void PushSubGameState(GameState* aSubGameState);
	void PushMainGameState(GameState* aMainGameState);
	bool UpdateCurrentState(const float& aDeltaTime);
	void RenderCurrentState();
	void ResumeState();
	void OnResizeCurrentState(int aWidth, int aHeight);
	void OnResize(int aWidth, int aHeight);
	void Clear();

	void SetCursor(GUI::Cursor* aCursor);

private:
	CU::GrowingArray<CU::GrowingArray<GameState*, int>, int> myGameStates;
	GUI::Cursor* myCursor;

	void PopSubGameState();
	void PopMainGameState();
	void RenderStateAtIndex(int aIndex);

	int myMainIndex;
	int mySubIndex;
	bool myShouldRender;

	StateStackProxy* myStateStackProxy;
};

inline void StateStack::SetCursor(GUI::Cursor* aCursor)
{
	myCursor = aCursor;
}