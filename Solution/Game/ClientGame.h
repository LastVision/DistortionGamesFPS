#pragma once
#include <SharedGame.h>
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

class ClientGame : public Subscriber, public SharedGame
{
public:
	ClientGame();
	~ClientGame();

	bool Init(HWND& aHwnd);
	bool Destroy();
	bool Update() override;

	void Pause();
	void UnPause();
	void OnResize(int aWidth, int aHeight);

	void ReceiveMessage(const FadeMessage& aMessage) override;

private:
	void operator=(ClientGame& aApp) = delete;

	GUI::Cursor* myCursor;

	HWND* myWindowHandler;

	StateStack myStateStack;

	bool myLockMouse;
	bool myShowSystemInfo;
	bool myIsComplete;
};