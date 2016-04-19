#pragma once
#include "NetImportantMessage.h"
#include <GrowingArray.h>
#include <GameEnum.h>

class NetMessageRequestConnect : public NetMessage
{
public:
	NetMessageRequestConnect(eGameType aGameType, const std::string& aName, short aServerID);
	NetMessageRequestConnect(eGameType aGameType, const std::string& aName, short aServerID, short aOtherClientID);
	NetMessageRequestConnect();

	~NetMessageRequestConnect();

	//void Init(const std::string& aName, short aServerID);

	std::string myName;
	unsigned int myServerID;
	unsigned int myOtherClientID;
	unsigned char myGameType;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageRequestConnect::NetMessageRequestConnect(eGameType aGameType, const std::string& aName, short aServerID)
	: NetMessage(eNetMessageType::ON_CONNECT)
	, myName(aName)
	, myServerID(aServerID)
{
	myGameType = static_cast<unsigned char>(aGameType);
}

inline NetMessageRequestConnect::NetMessageRequestConnect()
	: NetMessage(eNetMessageType::ON_CONNECT)
{
}

inline NetMessageRequestConnect::NetMessageRequestConnect(eGameType aGameType, const std::string& aName, short aServerID, short aOtherClientID)
	: NetMessage(eNetMessageType::ON_CONNECT)
	, myName(aName)
	, myServerID(aServerID)
	, myOtherClientID(aOtherClientID)
{
	myGameType = static_cast<unsigned char>(aGameType);
}

inline NetMessageRequestConnect::~NetMessageRequestConnect()
{
}

inline void NetMessageRequestConnect::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGameType);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
	SERIALIZE(aStream, myOtherClientID);
}

inline void NetMessageRequestConnect::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGameType);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
	DESERIALIZE(aStream, myOtherClientID);
}
