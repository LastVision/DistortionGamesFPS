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
	void Send(const std::vector<char>& anArray, const sockaddr_in& aTargetAddress);
	void Receieve(std::vector<Buffer>& someBuffers);

	bool ConnectToServer(const char* anIP);
	const CU::GrowingArray<OtherClients>& GetClientList();

	const sockaddr_in& GetServerAddress() const;
	const sockaddr_in& GetLocalServerAddress() const;

private:
	void Reset();

	//CleintInterface * myInstance;


	int myPingCount;
	WSADATA myWSAData;
	sockaddr_in myServerAddress;
	sockaddr_in myLocalServerAddress;

	uint16_t myPort;
	SOCKET mySocket;
	uint16_t myIDFromServer;

	std::string myName;
	std::string myIP;
	CU::GrowingArray<OtherClients> myClients;

};

inline const sockaddr_in& ClientNetwork::GetServerAddress() const
{
	return myServerAddress;
}

inline const sockaddr_in& ClientNetwork::GetLocalServerAddress() const
{
	return myLocalServerAddress;
}