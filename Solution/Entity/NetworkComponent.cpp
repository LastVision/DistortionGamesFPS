#include "stdafx.h"
#include "NetworkComponent.h"
#include "Entity.h"
#include <MathHelper.h>
#include <NetworkSetPositionMessage.h>
#include <NetworkSendPositionMessage.h>

#include <PostMaster.h>

NetworkComponent::NetworkComponent(Entity& anEntity, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myOrientation(anOrientation)
	, myAlpha(0.f)
{
	mySendTime = 1 * 0.033f;
	myFirstPosition = { 0.f, 0.f, 0.f };
	mySecondPosition = { 1.f, 0.5f, -56.f };
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_SET_POSITION, this);
}


NetworkComponent::~NetworkComponent()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_SET_POSITION, this);
}

const unsigned int NetworkComponent::GetNetworkID() const
{
	return myNetworkID;
}

void NetworkComponent::SetNetworkID(unsigned int anID)
{
	myNetworkID = anID;
}

void NetworkComponent::Update(float aDelta)
{
	myAlpha += aDelta * 1.f;
	if (myEntity.GetIsClient() == true)
	{
		CU::Vector3<float> newPos = CU::Math::Lerp(myPrevPosition, myServerPosition, myAlpha);
		myOrientation.SetPos(newPos);
	}
	else
	{
		mySendTime -= aDelta;
		if (mySendTime < 0.f)
		{
			//myServerPosition += aDelta * 5.f;
			if (myShouldReturn == false)
			{
				myServerPosition = CU::Math::Lerp(myFirstPosition, mySecondPosition, myAlpha);
			}
			else
			{
				myServerPosition = CU::Math::Lerp(mySecondPosition, myFirstPosition, myAlpha);

			}
			if (myAlpha >= 1)
			{
				myShouldReturn = !myShouldReturn;
				myAlpha = 0;
			}


			PostMaster::GetInstance()->SendMessage(NetworkSendPositionMessage(myServerPosition, myNetworkID));
			mySendTime = 1 * 0.033f;
		}
	}
}

void NetworkComponent::ReceiveMessage(const NetworkSetPositionMessage& aMessage)
{
	if (aMessage.myNetworkID == myNetworkID)
	{
		myPrevPosition = myServerPosition;
		myServerPosition = aMessage.myPosition;
		myAlpha = 0.f;
	}
}
