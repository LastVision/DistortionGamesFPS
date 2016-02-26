#pragma once
#include "../Game_Shared/SharedNetworkManager.h"
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>

class ServerNetwork;
class ServerNetworkManager : public SharedNetworkManager
{
public:
	void Initiate() override;
	static void Create();
	static void Destroy();
	static ServerNetworkManager* GetInstance();

	void StartNetwork() override;

private:
	ServerNetworkManager();
	~ServerNetworkManager();
	static ServerNetworkManager* myInstance;
	ServerNetwork* myNetwork;

	void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSender) override;

	void ReceieveThread() override;
	void SendThread() override;

	short myIDCount;
	void CreateConnection(const std::string& aName, const sockaddr_in& aSender);

	CU::GrowingArray<Connection> myClients;
	std::unordered_map<std::string, int> myNames;

};

