#pragma once
#include <GrowingArray.h>
#include "NetMessageOnJoin.h"
#include "NetworkMessageTypes.h"
class ServerNetwork 
{
public:
	ServerNetwork();
	~ServerNetwork();

	void StartServer();
	void Send(const std::vector<char>& anArray);

	void Receieve(std::vector<Buffer>& someBuffers);

	void CreateConnection(const std::string& aName);


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

