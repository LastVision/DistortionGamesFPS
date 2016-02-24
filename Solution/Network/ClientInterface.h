#pragma once
#include "BaseNetwork.h"
#include <GrowingArray.h>



class ClientInterface : public BaseNetwork
{
public:
	ClientInterface();
	~ClientInterface();
	//void Init(BaseNetwork* ptr) { myInstance = ptr);

	void StartNetwork() override;
	void Send(const std::vector<char>& anArray) override;
	void Receieve(std::vector<Buffer>& someBuffers) override;

	void ConnectToServer(const char* anIP) override;
	const CU::GrowingArray<OtherClients>& GetClientList();
private:

	//CleintInterface * myInstance;


	int myPingCount;
	WSADATA myWSAData;
	sockaddr_in myServerAddress;

	uint16_t myPort;
	uint16_t mySocket;
	uint16_t myIDFromServer;

	std::string myName;
	const char* myIP;
	CU::GrowingArray<OtherClients> myClients;

};

