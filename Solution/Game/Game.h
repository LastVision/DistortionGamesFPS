#pragma once
#include "StateStack.h"
#include <Subscriber.h>

namespace GUI
{
	class Cursor;
}

namespace Prism
{
	class Camera;
}

class Game : public Subscriber
{
public:
	Game();
	~Game();

	bool Init(HWND& aHwnd);
	bool Destroy();
	bool Update();

	void Pause();
	void UnPause();
	void OnResize(int aWidth, int aHeight);

	void ReceiveMessage(const FadeMessage& aMessage) override;

private:
	void operator=(Game& aApp) = delete;

	GUI::Cursor* myCursor;

	HWND* myWindowHandler;

	StateStack myStateStack;

	bool myLockMouse;
	bool myShowSystemInfo;
	bool myIsComplete;
};