#pragma once
#include "NetImportantMessage.h"
#include <GrowingArray.h>

class NetMessageRequestConnect : public NetImportantMessage
{
public:
	NetMessageRequestConnect(const std::string& aName, short aServerID);
	NetMessageRequestConnect(const std::string& aName, short aServerID, short aOtherClientID);
	NetMessageRequestConnect();

	~NetMessageRequestConnect();

	//void Init(const std::string& aName, short aServerID);

	std::string myName;
	unsigned int myServerID;
	unsigned int myOtherClientID;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageRequestConnect::NetMessageRequestConnect(const std::string& aName, short aServerID)
	: NetImportantMessage(eNetMessageType::ON_CONNECT)
	, myName(aName)
	, myServerID(aServerID)
{
}

inline NetMessageRequestConnect::NetMessageRequestConnect()
	: NetImportantMessage(eNetMessageType::ON_CONNECT)
{
}

inline NetMessageRequestConnect::NetMessageRequestConnect(const std::string& aName, short aServerID, short aOtherClientID)
	: NetImportantMessage(eNetMessageType::ON_CONNECT)
	, myName(aName)
	, myServerID(aServerID)
	, myOtherClientID(aOtherClientID)
{
}

inline NetMessageRequestConnect::~NetMessageRequestConnect()
{
}

inline void NetMessageRequestConnect::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
	SERIALIZE(aStream, myOtherClientID);
}

inline void NetMessageRequestConnect::DoDeSerialize(StreamType& aStream)
{

	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
	DESERIALIZE(aStream, myOtherClientID);
}
