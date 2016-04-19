#pragma once
#include "NetImportantMessage.h"
class NetMessageLoadLevel : public NetImportantMessage
{
public:
	NetMessageLoadLevel(int aLevelID, unsigned int aLevelHash);
	NetMessageLoadLevel();

	int myLevelID;
	unsigned int myLevelHash;
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageLoadLevel::NetMessageLoadLevel(int aLevelID, unsigned int aLevelHash)
	: NetImportantMessage(eNetMessageType::LOAD_LEVEL)
	, myLevelID(aLevelID)
	, myLevelHash(aLevelHash)
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
	SERIALIZE(aStream, myLevelHash);
}

inline void NetMessageLoadLevel::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myLevelID);
	DESERIALIZE(aStream, myLevelHash);
}
