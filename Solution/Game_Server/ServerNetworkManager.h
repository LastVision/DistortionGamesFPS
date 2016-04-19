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

	void Update(float aDelta) override;

	void ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;

	const short GetLastJoinedID() const;
	const CU::GrowingArray<Connection>& GetClients() const;

	const std::string& GetIP() const;

	void DisconnectAll();

	const bool IsAllowedNewConnection() const;

	void StopPing(const bool aStopFlag);
private:
	ServerNetworkManager();
	~ServerNetworkManager();
	ServerNetwork* myNetwork;
	void UpdateImportantMessages(float aDeltaTime) override;

	void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID) override;
	void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID, const sockaddr_in& aTargetAddress) override;

	void ReceieveThread() override;
	void SendThread() override;
	void PingThread() override;

	short myIDCount;
	float myPingTime;

	void DisconnectConnection(const Connection& aConnection);

	CU::GrowingArray<Connection> myClients;
	std::unordered_map<std::string, int> myNames;
	bool myAllowNewConnections;

	bool myStopPingFlag;
};

inline void ServerNetworkManager::AllowNewConnections(bool aValue)
{
	myAllowNewConnections = aValue;
}

inline const CU::GrowingArray<Connection>& ServerNetworkManager::GetClients() const
{
	return myClients;
}
inline const short ServerNetworkManager::GetLastJoinedID() const
{
	return myIDCount;
}

inline const bool ServerNetworkManager::IsAllowedNewConnection() const
{
	return myAllowNewConnections;
}

inline void ServerNetworkManager::StopPing(const bool aStopFlag)
{
	myStopPingFlag = aStopFlag;
}