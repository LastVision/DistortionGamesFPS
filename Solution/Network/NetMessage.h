#pragma once
#include "SerializeHelper.h"
#include <stdint.h>
#define SERIALIZE(aStream, aType) serialize(aType,aStream);
#define DESERIALIZE(aStream, aType) aType = deserialize<decltype(aType)>(aStream)
class NetMessage
{
public:
	NetMessage();
	virtual ~NetMessage();

	void PackMessage();
	void UnPackMessage(char* aMessage, int aSize);
	virtual bool GetIsImportant();
	virtual const unsigned int GetImportantID() const;

	StreamType myStream;
	uint32_t myTimeStamp;
	uint16_t mySenderID;
	uint16_t myTargetID;
	uint8_t myID;
protected:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeSerialize(StreamType& aStream);




};

