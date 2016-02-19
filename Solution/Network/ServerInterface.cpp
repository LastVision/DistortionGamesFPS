#include "stdafx.h"
#include "ServerInterface.h"
#include "Utility.h"
#define BUFFERSIZE 512

ServerInterface::ServerInterface()
{
}


ServerInterface::~ServerInterface()
{
	WSACleanup();
}

void ServerInterface::StartNetwork()
{
	myClients.Init(8);
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

	Utility::PrintEndl("Server successfully started!", Utility::eCOLOR::LIGHT_GREEN);
}

void ServerInterface::Send(const std::vector<char>& anArray)
{
	for (int i = 0; i < myClients.Size(); ++i)
	{
		if (sendto(myListenSocket, &anArray[0], anArray.size(), 0, (struct sockaddr *)&myClients[i].myAdress
			, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			std::string toPrint = "sendto() failed with error code : " + errorCode;
			Utility::PrintEndl(toPrint,Utility::eCOLOR::WHITE_BACK_RED);
		}
	}
}

int ServerInterface::Receieve(char* aBuffer)
{
	int toReturn = 0;
	int size = sizeof(myOther);
	if ((toReturn = recvfrom(myListenSocket, aBuffer, BUFFERSIZE, 0, (struct sockaddr *)&myOther,&size)) == SOCKET_ERROR)
	{
		//Error
	}
	return toReturn;
}
