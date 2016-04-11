#pragma once
#include "NetImportantMessage.h"
#include <Vector4.h>

class NetMessageText : public NetImportantMessage
{
public:
	NetMessageText(std::string aText, float aTime = 5.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f }, int aTextRows = 0, bool aIsMissionText = false, bool aShouldShow = true);
	NetMessageText();
	~NetMessageText(){};

	std::string myText;
	CU::Vector4<float> myColor;
	float myTime;
	bool myIsMissionText;
	bool myShouldShow;
	int myTextRows;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageText::NetMessageText(std::string aText, float aTime, CU::Vector4<float> aColor, int aTextRows, bool aIsMissionText, bool aShouldShow)
	: NetImportantMessage(eNetMessageType::TEXT)
	, myText(aText)
	, myTime(aTime)
	, myColor(aColor)
	, myTextRows(aTextRows)
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
	SERIALIZE(aStream, myColor);
	SERIALIZE(aStream, myTextRows);
	SERIALIZE(aStream, myIsMissionText);
	SERIALIZE(aStream, myShouldShow);
}

inline void NetMessageText::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myText);
	DESERIALIZE(aStream, myTime);
	DESERIALIZE(aStream, myColor);
	DESERIALIZE(aStream, myTextRows);
	DESERIALIZE(aStream, myIsMissionText);
	DESERIALIZE(aStream, myShouldShow);
}
