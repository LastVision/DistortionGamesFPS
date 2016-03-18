#pragma once
#include "NetMessage.h"
class NetImportantMessage : public NetMessage 
{
public:
	NetImportantMessage(eNetMessageType aID = eNetMessageType::NONE);
	virtual ~NetImportantMessage();

	bool GetIsImportant() const override;
	const unsigned int GetImportantID() const override;
	virtual void SetImportantID(unsigned int anImportantID);
protected:
	void DoSerialize(StreamType& aStream);
	void DoDeSerialize(StreamType& aStream);
	unsigned int myImportantID;


};

inline NetImportantMessage::NetImportantMessage(eNetMessageType aID)
	: NetMessage(aID)
{
}

inline NetImportantMessage::~NetImportantMessage()
{
}

inline bool NetImportantMessage::GetIsImportant() const
{
	return true;
}

inline const unsigned int NetImportantMessage::GetImportantID() const
{
	return myImportantID;
}

inline void NetImportantMessage::SetImportantID(unsigned int anImportantID)
{
	myImportantID = anImportantID;
}

inline void NetImportantMessage::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myImportantID);
}

inline void NetImportantMessage::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myImportantID);
}
