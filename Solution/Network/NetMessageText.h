#pragma once
#include "NetImportantMessage.h"

class NetMessageText : public NetImportantMessage
{
public:
	NetMessageText(std::string aText, float aTime = 5.f, bool aIsMissionText = false, bool aShouldShow = true);
	NetMessageText();
	~NetMessageText(){};

	std::string myText;
	float myTime;
	bool myIsMissionText;
	bool myShouldShow;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageText::NetMessageText(std::string aText, float aTime, bool aIsMissionText, bool aShouldShow)
	: NetImportantMessage(eNetMessageType::TEXT)
	, myText(aText)
	, myTime(aTime)
	, myIsMissionText(aIsMissionText)
	, myShouldShow(aShouldShow)
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
	SERIALIZE(aStream, myIsMissionText);
	SERIALIZE(aStream, myShouldShow);
}

inline void NetMessageText::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myText);
	DESERIALIZE(aStream, myTime);
	DESERIALIZE(aStream, myIsMissionText);
	DESERIALIZE(aStream, myShouldShow);
}
