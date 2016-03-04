#pragma once
#include <GrowingArray.h>
#include <stdint.h>
#include "NetworkMessageTypes.h"

class ClientNetwork
{
public:
	ClientNetwork();
	~ClientNetwork();

	void StartNetwork(int aPortNum);
	void Send(const std::vector<char>& anArray);
	void Receieve(std::vector<Buffer>& someBuffers);

	bool ConnectToServer(const char* anIP);
	const CU::GrowingArray<OtherClients>& GetClientList();
private:

	//CleintInterface * myInstance;


	int myPingCount;
	WSADATA myWSAData;
	sockaddr_in myServerAddress;

	uint16_t myPort;
	uint16_t mySocket;
	uint16_t myIDFromServer;

	std::string myName;
	const char* myIP;
	CU::GrowingArray<OtherClients> myClients;

};

