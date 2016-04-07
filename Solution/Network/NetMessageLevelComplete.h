#pragma once
#include "NetImportantMessage.h"

class NetMessageLevelComplete : public NetImportantMessage
{
public:
	NetMessageLevelComplete();
	NetMessageLevelComplete(bool aAllPlayersDied);

	bool myAllPlayersDied;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageLevelComplete::NetMessageLevelComplete()
	: NetImportantMessage(eNetMessageType::LEVEL_COMPLETE)
{
}

inline NetMessageLevelComplete::NetMessageLevelComplete(bool aAllPlayersDied)
	: NetImportantMessage(eNetMessageType::LEVEL_COMPLETE)
	, myAllPlayersDied(aAllPlayersDied)
{
}

inline void NetMessageLevelComplete::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myAllPlayersDied);
}

inline void NetMessageLevelComplete::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myAllPlayersDied);
}
