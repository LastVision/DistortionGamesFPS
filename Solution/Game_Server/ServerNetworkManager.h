#pragma once
#include "../PostMaster/Subscriber.h"
#include <SharedNetworkManager.h>

class ServerNetwork;
class ServerNetworkManager : public SharedNetworkManager, public Subscriber
{
public:
	void Initiate() override;
	static void Create();
	static void Destroy();
	static ServerNetworkManager* GetInstance();

	void StartNetwork(unsigned int aPortNum = 13397) override;

	bool ListContainsAllClients(const CU::GrowingArray<unsigned int>& someClientIDs) const;
	void AllowNewConnections(bool aValue);

	void CreateConnection(const std::string& aName, const sockaddr_in& aSender);


	void ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;

	const short GetLastJoinedID() const;
private:
	ServerNetworkManager();
	~ServerNetworkManager();
	static ServerNetworkManager* myInstance;
	ServerNetwork* myNetwork;
	void UpdateImportantMessages(float aDeltaTime) override;

	void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID) override;

	//void ReceiveMessage(const PostMasterNetAddEnemyMessage& aMessage) override;
	//void ReceiveMessage(const PostMasterNetSendPositionMessage& aMessage) override;
	//void ReceiveMessage(const PostMasterNetOnDeathMessage& aMessage) override;

	void ReceieveThread() override;
	void SendThread() override;

	short myIDCount;
	
	void DisconnectConnection(const Connection& aConnection);

	CU::GrowingArray<Connection> myClients;
	std::unordered_map<std::string, int> myNames;
	bool myAllowNewConnections;
};

inline void ServerNetworkManager::AllowNewConnections(bool aValue)
{
	myAllowNewConnections = aValue;
}

inline const short ServerNetworkManager::GetLastJoinedID() const
{
	return myIDCount;
}

