#pragma once
#include "NetImportantMessage.h"
#include <string>
class NetMessageOnJoin : public NetImportantMessage
{
public:
	NetMessageOnJoin();
	NetMessageOnJoin(const std::string& aName, unsigned int aOtherClientID);
	~NetMessageOnJoin();

	std::string myName;
	unsigned int myOtherClientID;
protected:
	
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageOnJoin::NetMessageOnJoin(const std::string& aName, unsigned int aOtherClientID)
	: NetImportantMessage(eNetMessageType::ON_JOIN)
	, myName(aName)
{
	myOtherClientID = aOtherClientID;
}

inline NetMessageOnJoin::NetMessageOnJoin()
	: NetImportantMessage(eNetMessageType::ON_JOIN)
	, myName("Not Initialized")
{
}

inline NetMessageOnJoin::~NetMessageOnJoin()
{
}

inline void NetMessageOnJoin::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myOtherClientID);
}

inline void NetMessageOnJoin::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myOtherClientID);
}
