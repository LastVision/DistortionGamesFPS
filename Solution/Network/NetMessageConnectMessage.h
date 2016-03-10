#pragma once
#include "NetImportantMessage.h"
#include <GrowingArray.h>

class NetMessageConnectMessage : public NetImportantMessage
{
public:
	NetMessageConnectMessage(const std::string& aName, short aServerID);
	NetMessageConnectMessage(const std::string& aName, short aServerID, short aOtherClientID);
	NetMessageConnectMessage();

	NetMessageConnectMessage(sockaddr_in anAddress);
	~NetMessageConnectMessage();

	//void Init(const std::string& aName, short aServerID);

	std::string myName;
	short myServerID;
	CU::GrowingArray<unsigned short> myClientsOnServer;
	unsigned short myOtherClientID;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

