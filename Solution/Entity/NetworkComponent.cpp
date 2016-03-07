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
	, myIsPlayer(false)
{
	mySendTime = 1 * 0.033f;
	myFirstPosition = myOrientation.GetPos();
	mySecondPosition = { 1.f, 0.5f, -56.f };
	mySecondPosition2 = { 0.f, 0.f, 0.f };
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
	myAlpha += aDelta * 0.25f;
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
			if (myIsPlayer == false)
			{
				if (myShouldReturn == false)
				{
					if (myFirstPosition.z < 0.f)
					{
						myServerPosition = CU::Math::Lerp(myFirstPosition, mySecondPosition2, myAlpha);
					}
					else
					{
						myServerPosition = CU::Math::Lerp(myFirstPosition, mySecondPosition, myAlpha);
					}
				}
				else
				{
					if (myFirstPosition.z < 0.f)
					{
						myServerPosition = CU::Math::Lerp(mySecondPosition2, myFirstPosition, myAlpha);
					}
					else
					{
						myServerPosition = CU::Math::Lerp(mySecondPosition, myFirstPosition, myAlpha);
					}
				}
				if (myAlpha >= 1)
				{
					myShouldReturn = !myShouldReturn;
					myAlpha = 0;
				}
			}
			else
			{
				int apa = 0;
			}


			mySendTime = 1 * 0.033f;
			PostMaster::GetInstance()->SendMessage(NetworkSendPositionMessage(myServerPosition, myNetworkID));
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

void NetworkComponent::SetPlayer(bool aBool)
{
	myIsPlayer = aBool;
}
