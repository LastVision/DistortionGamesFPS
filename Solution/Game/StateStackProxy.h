#pragma once
class StateStack;
class GameState;

class StateStackProxy
{
public:
	StateStackProxy(StateStack& aStateStack);

	void PushSubGameState(GameState* aGameState);
	void PushMainGameState(GameState* aGameState);

private:
	StateStackProxy& operator=(const StateStackProxy&);
	StateStack& myStateStack;
};