#pragma once

#include <Subscriber.h>
#include <NetworkSubscriber.h>
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

namespace Prism
{
	class TextProxy;
	class SpriteProxy;
}

class LobbyState : public GameState, public Subscriber, public NetworkSubscriber
{
public:
	LobbyState(bool aShowStartButton);
	~LobbyState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;
	void ReceiveMessage(const OnRadioButtonMessage& aMessage) override;
	void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageLoadLevel& aMessage, const sockaddr_in& aSenderAddress) override;

private:
	GUI::GUIManager* myGUIManagerHost;
	GUI::GUIManager* myGUIManager;
	Prism::TextProxy* myText;
	Prism::TextProxy* myWaitingForJoinText;
	Prism::TextProxy* myPlayerListText;
	Prism::TextProxy* myWaitingForHostText;
	Prism::SpriteProxy* myRotatingThingy;

	bool myShowStartButton;

	int myLevelToStart;
	unsigned int myServerLevelHash;
	bool myStartGame;

	float myRefreshPlayerListTimer;
	float myWaitingForConnectTimer;

	int myRadioLevel;
	int myRadioDifficulty;
};

