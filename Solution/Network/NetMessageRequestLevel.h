#pragma once
#include "NetImportantMessage.h"

class NetMessageRequestLevel : public NetImportantMessage
{
public:
	NetMessageRequestLevel(int aLevelID);
	NetMessageRequestLevel();
	~NetMessageRequestLevel();

	int myLevelID;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageRequestLevel::NetMessageRequestLevel()
	: NetImportantMessage(eNetMessageType::REQUEST_LEVEL)
{
}

inline NetMessageRequestLevel::NetMessageRequestLevel(int aLevelID)
	: NetImportantMessage(eNetMessageType::REQUEST_LEVEL)
	, myLevelID(aLevelID)
{
}

inline NetMessageRequestLevel::~NetMessageRequestLevel()
{
}

inline void NetMessageRequestLevel::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myLevelID);
}

inline void NetMessageRequestLevel::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myLevelID);
}
