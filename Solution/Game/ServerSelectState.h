#pragma once

#include <NetworkSubscriber.h>
#include <Subscriber.h>
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class ServerSelectState : public GameState, public Subscriber, public NetworkSubscriber
{
public:
	ServerSelectState();
	~ServerSelectState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;

	void ReceiveNetworkMessage(const NetMessageReplyServer& aMessage, const sockaddr_in& aSenderAddress) override;

private:
	struct Server
	{
		std::string myIp;
		std::string myName;

		bool operator==(const Server& aServer){ return (aServer.myIp == myIp && aServer.myName == myName); }
	};
	GUI::GUIManager* myGUIManager;

	CU::GrowingArray<Server> myServers;

	Server* myServer;

	float myRefreshServerTimer;

	bool myTriedToConnect;
};

