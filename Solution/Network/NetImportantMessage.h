#pragma once
#include "NetMessage.h"
class NetImportantMessage : public NetMessage 
{
public:
	NetImportantMessage();
	virtual ~NetImportantMessage();

	bool GetIsImportant() const override;
	const unsigned int GetImportantID() const override;
	virtual void SetImportantID(unsigned int anImportantID);
protected:
	void DoSerialize(StreamType& aStream);
	void DoDeSerialize(StreamType& aStream);
	unsigned int myImportantID;


};

