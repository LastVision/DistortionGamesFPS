#include "stdafx.h"
#include "ServerInterface.h"
#include "Utility.h"
#include "NetMessageConnectMessage.h"
#include "NetMessageOnJoin.h"
#define BUFFERSIZE 512

short ServerInterface::myIDCount = 0;

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

	DWORD nonBlocking = 1;
	if (ioctlsocket(myListenSocket, FIONBIO, &nonBlocking) != 0)
	{
		Utility::PrintEndl("Failed to set non-blocking socket!", Utility::eCOLOR::WHITE_BACK_RED);
	}
	else
	{
		Utility::PrintEndl("Successfully set up non-blocking socket!", Utility::eCOLOR::LIGHT_GREEN);
	}


	myIsOnline = true;
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
			Utility::PrintEndl(toPrint, Utility::eCOLOR::WHITE_BACK_RED);
		}
	}
}

void ServerInterface::Send(const std::vector<char>& anArray, const sockaddr_in& anAddress)
{
	if (sendto(myListenSocket, &anArray[0], anArray.size(), 0, (struct sockaddr *)&anAddress
		, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		std::string toPrint = "sendto() failed with error code : " + errorCode;
		Utility::PrintEndl(toPrint, Utility::eCOLOR::WHITE_BACK_RED);
	}
}

void ServerInterface::Send(NetMessageOnJoin join)
{
	join.Init();
	join.mySenderID = 0;

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
	}
}

void ServerInterface::Receieve(std::vector<Buffer>& someBuffers)
{
	int toReturn = 0;
	int size = sizeof(myOther);
	char* receieve = nullptr;

	char buffer[BUFFERSIZE];
	ZeroMemory(&buffer, BUFFERSIZE);
	Buffer toPushback;

	while ((toReturn = recvfrom(myListenSocket, buffer, BUFFERSIZE, 0, (struct sockaddr *)&myOther, &size)) > 0)
	{
		memcpy(&toPushback.myData, &buffer[0], toReturn*sizeof(char));
		toPushback.myLength = toReturn;
		someBuffers.push_back(toPushback);
	}

	//return toReturn;
}

void ServerInterface::CreateConnection(const std::string& aName)
{
	if (myNames.find(aName) == myNames.end())
	{
		myIDCount++;
		Connection newConnection;
		newConnection.myAdress = myOther;
		newConnection.myID = myIDCount;
		newConnection.myName = aName;
		newConnection.myPingCount = 0;
		newConnection.myIsConnected = true;
		myClients.Add(newConnection);
		myNames[aName] = 1;


		NetMessageConnectMessage toSend;
		toSend.Init(aName, myIDCount);
		toSend.PackMessage();
		Send(toSend.myStream, newConnection.myAdress);

		NetMessageOnJoin onJoin;
		Send(onJoin);
	}
	else
	{
		Utility::PrintEndl("User is already on server!", Utility::eCOLOR::RED_BACK_BLACK);
	}
}
