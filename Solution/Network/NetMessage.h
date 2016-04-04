#pragma once
#include "SerializeHelper.h"
#include <stdint.h>
#include "NetworkMessageTypes.h"
#define SERIALIZE(aStream, aType) serialize(aType,aStream);
#define DESERIALIZE(aStream, aType) aType = deserialize<decltype(aType)>(aStream)
class NetMessage
{
public:
	NetMessage(eNetMessageType aID = eNetMessageType::NONE);
	virtual ~NetMessage();

	void PackMessage();
	void UnPackMessage(char* aMessage, int aSize);
	virtual bool GetIsImportant() const;
	virtual const unsigned int GetImportantID() const;
	virtual void SetImportantID(unsigned int){}

	void DeSerializeFirst(char* aMessage);


	StreamType myStream;
	uint32_t myTimeStamp;
	uint32_t mySenderID;
	uint32_t myTargetID;
	uint16_t myGameID;
	uint8_t myID;
protected:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeSerialize(StreamType& aStream);
	virtual void DoDeSerializeFirst(StreamType& aStream);



};

