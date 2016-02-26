#pragma once

#include <Subscriber.h>
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class LobbyState : public GameState, public Subscriber
{
public:
	LobbyState();
	~LobbyState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:
	struct Server
	{
		std::string myIp;
		std::string myName;
	};
	GUI::GUIManager* myGUIManager;

	CU::GrowingArray<Server> myServers;

	Server* myServer;
};

