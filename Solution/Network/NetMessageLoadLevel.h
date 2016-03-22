#pragma once
#include "NetImportantMessage.h"
class NetMessageLoadLevel : public NetImportantMessage
{
public:
	NetMessageLoadLevel(int aLevelID);
	NetMessageLoadLevel();

	int myLevelID;

protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageLoadLevel::NetMessageLoadLevel(int aLevelID)
	: NetImportantMessage(eNetMessageType::LOAD_LEVEL)
	, myLevelID(aLevelID)
{
}

inline NetMessageLoadLevel::NetMessageLoadLevel()
	: NetImportantMessage(eNetMessageType::LOAD_LEVEL)
	, myLevelID(INT_MAX)
{
}

inline void NetMessageLoadLevel::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myLevelID);
}

inline void NetMessageLoadLevel::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myLevelID);
}
