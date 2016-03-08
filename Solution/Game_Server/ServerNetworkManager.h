#pragma once
#include "../PostMaster/Subscriber.h"
#include "../Game_Shared/SharedNetworkManager.h"

class ServerNetwork;
class ServerNetworkManager : public SharedNetworkManager, public Subscriber
{
public:
	void Initiate() override;
	static void Create();
	static void Destroy();
	static ServerNetworkManager* GetInstance();

	void StartNetwork(unsigned int aPortNum = 13397) override;
private:
	ServerNetworkManager();
	~ServerNetworkManager();
	static ServerNetworkManager* myInstance;
	ServerNetwork* myNetwork;

	void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSender) override;
	void HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress) override;


	void ReceiveMessage(const NetworkAddEnemyMessage& aMessage) override;
	void ReceiveMessage(const NetworkSendPositionMessage& aMessage) override;

	void ReceieveThread() override;
	void SendThread() override;

	short myIDCount;
	void CreateConnection(const std::string& aName, const sockaddr_in& aSender);

	CU::GrowingArray<Connection> myClients;
	std::unordered_map<std::string, int> myNames;

};

