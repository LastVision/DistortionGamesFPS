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
	unsigned int GetGID() const;
	const CU::GrowingArray<OtherClients>& GetClients();
	//void Update(float aDelta) override;
	void ReceiveMessage(const PostMasterNetSendPositionMessage& aMessage) override;
	void ReceiveMessage(const PostMasterNetOnDisconnectMessage& aMessage) override;
	void ReceiveMessage(const PostMasterNetOnHitMessage& aMessage) override;

	void DebugPrint();

private:
	ClientNetworkManager();
	~ClientNetworkManager();
	static ClientNetworkManager* myInstance;
	//void AddNetworkMessage(std::vector<char> aBuffer) override;
	void UpdateImportantMessages(float aDeltaTime) override;

	void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID) override;

	ClientNetwork* myNetwork;
	void HandleMessage(const NetMessageConnectReply& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageAddEnemy& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress) override;
	void HandleMessage(const NetMessageStartGame& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceieveThread() override;
	void SendThread() override;


	CU::GrowingArray<OtherClients> myClients;
	std::string myName;
};

