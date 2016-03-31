#pragma once
#include <GrowingArray.h>
#include "NetMessageOnJoin.h"
#include "NetworkMessageTypes.h"
class ServerNetwork 
{
public:
	ServerNetwork();
	~ServerNetwork();

	void StartServer(unsigned int aPortNum);

	void Send(const std::vector<char>& anArray, const sockaddr_in& anAddress);

	void Receieve(std::vector<Buffer>& someBuffers);

	void PrintStatus();

	const std::string& GetIP() const;
private:

	void Send(NetMessageOnJoin join);


	sockaddr_in myServer;

	SOCKET myListenSocket;
	WSADATA myWSAData;

	std::string myPort;
	std::string myIP;
};

inline const std::string& ServerNetwork::GetIP() const
{
	return myIP;
}