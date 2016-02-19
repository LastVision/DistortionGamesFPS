#include "stdafx.h"
#include "ClientInterface.h"
#include "NetMessageConnectMessage.h"
#include <DL_Debug.h>
#define BUFFERSIZE 512

ClientInterface::ClientInterface()
{
}


ClientInterface::~ClientInterface()
{
	closesocket(mySocket);
	WSACleanup();
}

void ClientInterface::StartNetwork()
{
	myPort = 13397;
	if (WSAStartup(MAKEWORD(2, 2), &myWSAData) != 0)
	{
		//Error
	}

	if ((mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		//Error
	}

	ZeroMemory(&myServerAddress, sizeof(myServerAddress));
	myServerAddress.sin_family = AF_INET;
	myServerAddress.sin_port = htons(myPort);
}

void ClientInterface::Send(const std::vector<char>& anArray)
{
	if (sendto(mySocket, &anArray[0], anArray.size(), 0, (struct sockaddr *) &myServerAddress, sizeof(myServerAddress)) == SOCKET_ERROR)
	{
		DL_ASSERT("Failed to send");
	}
}

int ClientInterface::Receieve(char* aBuffer)
{
	int toReturn;
	if ((toReturn = recv(mySocket, aBuffer, BUFFERSIZE, 0)) == SOCKET_ERROR)
	{
		//Error
	}
	return toReturn;
}

void ClientInterface::ConnectToServer(const char* anIP)
{
	myIP = anIP;

	std::string serverIP = "127.0.0.1";
	myServerAddress.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());

	char username[256 + 1];
	DWORD username_len = 256 + 1;
	GetUserNameA(username, &username_len);
	myName = username;

	NetMessageConnectMessage toSend;
	toSend.Init(myName, -1);
	toSend.PackMessage();
	Send(toSend.myStream);

}
