#include "stdafx.h"
#include "ServerNetwork.h"
#include "Utility.h"
#include "NetMessageConnectMessage.h"
#include "NetMessageOnJoin.h"
#define BUFFERSIZE 512

ServerNetwork::ServerNetwork()
{
}


ServerNetwork::~ServerNetwork()
{
	WSACleanup();
}

void ServerNetwork::StartServer()
{
	//myClients.Init(8);
	myPort = "13397";
	addrinfo* addrResult;
	addrinfo  hints;

	ZeroMemory(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	int result;
	result = WSAStartup(MAKEWORD(2, 2), &myWSAData);
	if (result != 0)
	{
		WSACleanup();
		assert(result == 0 && "WSAStartup failed. Invalid data.");
	}
	else
	{
		std::cout << "WSAStartup succeeded!\n";
	}

	result = getaddrinfo(nullptr, myPort, &hints, &addrResult);
	if (result != 0)
	{
		freeaddrinfo(addrResult);
		WSACleanup();
		assert(result == 0 && "Failed to get address information!");
	}
	myListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (myListenSocket == INVALID_SOCKET)
	{
		freeaddrinfo(addrResult);
		WSACleanup();
		assert(myListenSocket != INVALID_SOCKET && "Invalid socket!");
	}

	result = bind(myListenSocket, addrResult->ai_addr, static_cast<int>(addrResult->ai_addrlen));

	if (result == INVALID_SOCKET)
	{
		freeaddrinfo(addrResult);
		WSACleanup();
		assert(result != INVALID_SOCKET && "Failed to bind socket!");
	}

	freeaddrinfo(addrResult);

	DWORD nonBlocking = 1;
	if (ioctlsocket(myListenSocket, FIONBIO, &nonBlocking) != 0)
	{
		Utility::PrintEndl("Failed to set non-blocking socket!", (WHITE_BACK | DARK_RED_TEXT));
	}
	else
	{
		Utility::PrintEndl("Successfully set up non-blocking socket!", LIGHT_GREEN_TEXT);
	}


	Utility::PrintEndl("Server successfully started!", LIGHT_GREEN_TEXT);

}

void ServerNetwork::Send(const std::vector<char>& anArray)
{
	/*for (int i = 0; i < myClients.Size(); ++i)
	{
	Send(anArray, myClients[i].myAdress);
	}*/
}

void ServerNetwork::Send(const std::vector<char>& anArray, const sockaddr_in& anAddress)
{
	if (sendto(myListenSocket, &anArray[0], anArray.size(), 0, (struct sockaddr *)&anAddress
		, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		std::string toPrint = "sendto() failed with error code : " + errorCode;
		Utility::PrintEndl(toPrint, (WHITE_BACK | DARK_RED_TEXT));
	}
}

void ServerNetwork::Send(NetMessageOnJoin join)
{
	/*join.Init();
	join.mySenderID = myIDCount;

	for (int i = 0; i < myClients.Size(); ++i)
	{
	if (myClients[i].myID != myIDCount)
	{
	join.myTargetID = myClients[i].myID;
	join.PackMessage();
	if (sendto(myListenSocket, &join.myStream[0], join.myStream.size(), 0, (struct sockaddr *)&myClients[i].myAdress
	, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
	int errorCode = WSAGetLastError();
	std::string toPrint = "sendto() failed with error code : " + errorCode;
	Utility::PrintEndl(toPrint, Utility::eCOLOR::WHITE_BACK_RED);
	}
	}
	}*/
}

void ServerNetwork::Receieve(std::vector<Buffer>& someBuffers)
{
	int toReturn = 0;
	int size = sizeof(sockaddr_in);

	char buffer[BUFFERSIZE];
	ZeroMemory(&buffer, BUFFERSIZE);

	Buffer toPushback;
	while ((toReturn = recvfrom(myListenSocket, buffer, BUFFERSIZE, 0, (struct sockaddr *)&toPushback.mySenderAddress, &size)) > 0)
	{
		memcpy(&toPushback.myData, &buffer[0], toReturn*sizeof(char));
		toPushback.myLength = toReturn;
		someBuffers.push_back(toPushback);
	}
}

