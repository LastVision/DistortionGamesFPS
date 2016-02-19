#pragma once
#include "BaseNetwork.h"
#include <GrowingArray.h>
class ServerInterface : public BaseNetwork
{
public:
	ServerInterface();
	~ServerInterface();

	void StartNetwork() override;
	void Send(const std::vector<char>& anArray) override;
	int Receieve(char* aBuffer) override;

private:

	sockaddr_in myServer;
	sockaddr_in myOther;

	SOCKET myListenSocket;
	WSADATA myWSAData;

	const char* myPort;

	struct Connection
	{
		sockaddr_in myAdress;
		std::string myName;
		int myPingCount;
		short myID;
		bool myIsConnected;
	};

	static short myIDCount;

	CU::GrowingArray<Connection> myClients;
	std::unordered_map<std::string, int> myNames;

};
