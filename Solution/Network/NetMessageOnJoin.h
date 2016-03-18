#pragma once
#include "NetImportantMessage.h"
#include <string>
class NetMessageOnJoin : public NetImportantMessage
{
public:
	NetMessageOnJoin();
	NetMessageOnJoin(const std::string& aName, unsigned int aSenderID);
	~NetMessageOnJoin();

	std::string myName;

protected:
	
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageOnJoin::NetMessageOnJoin(const std::string& aName, unsigned int aSenderID)
	: NetImportantMessage(eNetMessageType::ON_JOIN)
	, myName(aName)
{
	mySenderID = aSenderID;
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
}

inline void NetMessageOnJoin::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myName);
}
