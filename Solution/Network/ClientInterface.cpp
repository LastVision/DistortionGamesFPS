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

void ClientInterface::Receieve(std::vector<Buffer>& someBuffers)
{
	int toReturn;
	char buffer[BUFFERSIZE];
	ZeroMemory(&buffer, BUFFERSIZE);
	Buffer toPushback;
	while ((toReturn = recv(mySocket, buffer, BUFFERSIZE, 0)) > 0)
	{
		memcpy(&toPushback.myData, &buffer[0], toReturn*sizeof(char));
		toPushback.myLength = toReturn;
		someBuffers.push_back(toPushback);
	}
}

void ClientInterface::ConnectToServer(const char* anIP)
{
	myIP = anIP;

	
	myServerAddress.sin_addr.S_un.S_addr = inet_addr(myIP);

	char username[256 + 1];
	DWORD username_len = 256 + 1;
	GetUserNameA(username, &username_len);
	myName = username;

	NetMessageConnectMessage toSend;
	toSend.Init(myName, -1);
	toSend.PackMessage();
	Send(toSend.myStream);

	DWORD nonBlocking = 1;
	if (ioctlsocket(mySocket, FIONBIO, &nonBlocking) != 0)
	{
		DL_ASSERT("Failed to set non-blocking socket!");
	}
	myIsOnline = true;

}

const CU::GrowingArray<OtherClients>& ClientInterface::GetClientList()
{
	return myClients;
}
