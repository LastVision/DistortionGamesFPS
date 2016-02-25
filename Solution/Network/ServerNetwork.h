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


private:

	void Send(NetMessageOnJoin join);
	void Send(const std::vector<char>& anArray, const sockaddr_in& anAddress);


	sockaddr_in myServer;

	SOCKET myListenSocket;
	WSADATA myWSAData;

	const char* myPort;


	

};

