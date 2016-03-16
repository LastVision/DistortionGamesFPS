#pragma once
#include "NetImportantMessage.h"
#include <GrowingArray.h>

class NetMessageRequestConnect : public NetImportantMessage
{
public:
	NetMessageRequestConnect(const std::string& aName, short aServerID);
	NetMessageRequestConnect(const std::string& aName, short aServerID, short aOtherClientID);
	NetMessageRequestConnect();

	NetMessageRequestConnect(sockaddr_in anAddress);
	~NetMessageRequestConnect();

	//void Init(const std::string& aName, short aServerID);

	std::string myName;
	unsigned int myServerID;
	unsigned int myOtherClientID;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

