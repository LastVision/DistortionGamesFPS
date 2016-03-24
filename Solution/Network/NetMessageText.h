#pragma once
#include "NetImportantMessage.h"

class NetMessageText : public NetImportantMessage
{
public:
	NetMessageText(std::string aText, float aTime = 5.f);
	NetMessageText();
	~NetMessageText(){};

	std::string myText;
	float myTime;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageText::NetMessageText(std::string aText, float aTime)
	: NetImportantMessage(eNetMessageType::TEXT)
	, myText(aText)
	, myTime(aTime)
{
}

inline NetMessageText::NetMessageText()
	: NetImportantMessage(eNetMessageType::TEXT)
{
}

inline void NetMessageText::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myText);
	SERIALIZE(aStream, myTime);
}

inline void NetMessageText::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myText);
	DESERIALIZE(aStream, myTime);
}
