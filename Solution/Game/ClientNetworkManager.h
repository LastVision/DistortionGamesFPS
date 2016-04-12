#pragma once
#include <SharedNetworkManager.h>
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
	void ConnectToServer(eGameType aType, const char* aServerIP = "127.0.0.1");
	unsigned int GetGID() const;
	const CU::GrowingArray<OtherClients>& GetClients();

	void Update(float aDeltaTime) override;

	void ReceiveNetworkMessage(const NetMessageConnectReply& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress) override;

	void DebugPrint();

	const std::string& GetName() const;

	const sockaddr_in& GetLocalServerAddress() const;
	void SetHasLeftLobby(const bool aBool);
	const bool& GetHasLeftLobby() const;
private:
	ClientNetworkManager();
	~ClientNetworkManager();

	void UpdateImportantMessages(float aDeltaTime) override;
	void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID) override;
	void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID, const sockaddr_in& aTargetAddress) override;

	void ReceieveThread() override;
	void SendThread() override;
	void PingThread() override;

	ClientNetwork* myNetwork;

	CU::GrowingArray<OtherClients> myClients;
	std::string myName;

	bool myHasLeftLobby;
};

inline const std::string& ClientNetworkManager::GetName() const
{
	return myName;
}

inline void ClientNetworkManager::SetHasLeftLobby(const bool aBool)
{
	myHasLeftLobby = aBool;
}

inline const bool& ClientNetworkManager::GetHasLeftLobby() const
{
	return myHasLeftLobby;
}