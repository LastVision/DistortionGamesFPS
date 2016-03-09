#pragma once
#include "../Game_Shared/SharedNetworkManager.h"
#include <Subscriber.h>
class ClientNetwork;
class ClientNetworkManager : public SharedNetworkManager, public Subscriber
{
public:

	void Initiate() override;
	static void Create();
	static void Destroy();
	static ClientNetworkManager* GetInstance();

	void StartNetwork(unsigned int aPortNum = 13397) override;
	void ConnectToServer(const char* aServerIP = "127.0.0.1");
	unsigned int GetNetworkID() const;
	const CU::GrowingArray<OtherClients>& GetClients();
	//void Update(float aDelta) override;
	void ReceiveMessage(const NetworkSendPositionMessage& aMessage) override;

private:
	ClientNetworkManager();
	~ClientNetworkManager();
	static ClientNetworkManager* myInstance;
	//void AddNetworkMessage(std::vector<char> aBuffer) override;

	ClientNetwork* myNetwork;
	void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageAddEnemy& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceieveThread() override;
	void SendThread() override;


	CU::GrowingArray<OtherClients> myClients;

};

