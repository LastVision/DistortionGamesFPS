#include "stdafx.h"
#include "ServerNetwork.h"
#include "Utility.h"
#include "NetMessageRequestConnect.h"
#include "NetMessageOnJoin.h"
#define BUFFERSIZE 512

ServerNetwork::ServerNetwork()
{
}


ServerNetwork::~ServerNetwork()
{
	WSACleanup();
}

void ServerNetwork::StartServer(unsigned int aPortNum)
{
	//myClients.Init(8);
	myPort = std::to_string(aPortNum);
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
#ifdef _DEBUG
		std::cout << "WSAStartup succeeded!\n";
#endif
	}

	result = getaddrinfo(nullptr, myPort.c_str(), &hints, &addrResult);
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
		Utility::Printf("Failed to set non-blocking socket!", (WHITE_BACK | DARK_RED_TEXT));
	}
	else
	{
		Utility::Printf("Successfully set up non-blocking socket!", LIGHT_GREEN_TEXT);
	}


	Utility::Printf("Server successfully started!", LIGHT_GREEN_TEXT);
	hostent* localHosy = gethostbyname("");
	myIP = inet_ntoa(*(struct in_addr*)*localHosy->h_addr_list);
}

void ServerNetwork::Send(const std::vector<char>& anArray, const sockaddr_in& anAddress)
{
	if (sendto(myListenSocket, &anArray[0], anArray.size(), 0, (struct sockaddr *)&anAddress
		, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		std::string toPrint = "sendto() failed with error code : " + errorCode;
		Utility::Printf(toPrint, (WHITE_BACK | DARK_RED_TEXT));
	}
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

void ServerNetwork::PrintStatus()
{
	std::string toPrint = "------ SERVER ONLINE ------\n------- SERVER INFO -------\n";
	toPrint += "Server IP: " + std::string(myIP) + "\nServer Port: " + myPort + "\n---------------------------\n";
	Utility::Printf(toPrint, eConsoleColor::LIGHT_BLUE_TEXT, true);
}
