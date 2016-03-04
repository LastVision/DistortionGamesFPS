#pragma once
#include "NetMessage.h"
#include <Vector3.h>
class NetMessageAddEnemy : public NetMessage
{
public:
	NetMessageAddEnemy(const CU::Vector3<float> &aPosition);
	NetMessageAddEnemy();
	~NetMessageAddEnemy();


	CU::Vector3<float> myPosition;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

