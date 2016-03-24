#pragma once
#include "NetImportantMessage.h"

class NetMessageSetLevel : public NetImportantMessage
{
public:
	NetMessageSetLevel(int aLevelID);
	NetMessageSetLevel();
	~NetMessageSetLevel();

	int myLevelID;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageSetLevel::NetMessageSetLevel()
	: NetImportantMessage(eNetMessageType::SET_LEVEL)
{
}

inline NetMessageSetLevel::NetMessageSetLevel(int aLevelID)
	: NetImportantMessage(eNetMessageType::SET_LEVEL)
	, myLevelID(aLevelID)
{
}

inline NetMessageSetLevel::~NetMessageSetLevel()
{
}

inline void NetMessageSetLevel::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myLevelID);
}

inline void NetMessageSetLevel::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myLevelID);
}
