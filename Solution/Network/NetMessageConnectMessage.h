#pragma once
#include "NetMessage.h"
class CNetMessageConnectMessage : public CNetMessage
{
public:
	CNetMessageConnectMessage();
	~CNetMessageConnectMessage() override;

	void Init(const std::string& aName, short aServerID);

	std::string myName;
	short myServerID;
protected:


	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

