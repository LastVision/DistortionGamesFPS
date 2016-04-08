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
	enum class eType
	{
		SINGLEPLAYER,
		MULTIPLAYER_HOST,
		MULTIPLAYER_JOIN,
	};
	ServerSelectState(eType aType);
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
	void operator=(ServerSelectState&) = delete;

	struct Server
	{
		std::string myIp;
		std::string myName;

		bool operator==(const Server& aServer){ return (aServer.myIp == myIp && aServer.myName == myName); }
	};
	GUI::GUIManager* myGUIManager;

	const eType myType;

	CU::GrowingArray<Server> myServers;

	Prism::TextProxy* mySearchingForServers;

	Server* myServer;
	Server myLocalhost;

	float myRefreshServerTimer;
	float myWaitForResponseTimer;

	bool myTriedToConnect;
};

