#pragma once
#include "BaseNetwork.h"
#include <GrowingArray.h>
#include "NetMessageOnJoin.h"
#include "NetworkMessageTypes.h"
class ServerInterface : public BaseNetwork
{
public:
	ServerInterface();
	~ServerInterface();

	void StartNetwork() override;
	void Send(const std::vector<char>& anArray) override;

	void Receieve(std::vector<Buffer>& someBuffers) override;

	void CreateConnection(const std::string& aName);

	void DontSendToID(const std::vector<char>& anArray, short anIDToNotSendToffsDanne);

private:

	void Send(NetMessageOnJoin join);
	void Send(const std::vector<char>& anArray, const sockaddr_in& anAddress);


	sockaddr_in myServer;
	sockaddr_in myOther;

	SOCKET myListenSocket;
	WSADATA myWSAData;

	const char* myPort;

	static short myIDCount;

	CU::GrowingArray<Connection> myClients;
	std::unordered_map<std::string, int> myNames;

};

