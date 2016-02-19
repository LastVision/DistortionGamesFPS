#pragma once
#include "BaseNetwork.h"
class ClientInterface : public BaseNetwork
{
public:
	ClientInterface();
	~ClientInterface();

	void StartNetwork() override;
	void Send(const std::vector<char>& anArray) override;
	int Receieve(char* aBuffer) override;

	void ConnectToServer(const char* anIP) override;

private:

	bool myIsConnected;
	int myPingCount;
	WSADATA myWSAData;
	sockaddr_in myServerAddress;

	uint16_t myPort;
	uint16_t mySocket;
	uint16_t myIDFromServer;

	std::string myName;
	const char* myIP;


};

