#include "stdafx.h"
#include "NetworkComponent.h"
#include "Entity.h"
#include <MathHelper.h>

#include <NetworkSetPositionMessage.h>
#include <NetworkSendPositionMessage.h>
#include <NetworkOnHitMessage.h>
#include <NetworkOnDeathMessage.h>
#include "DamageNote.h"

#include <PostMaster.h>
#include "HealthComponent.h"
#include <Quaternion.h>

NetworkComponent::NetworkComponent(Entity& anEntity, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myOrientation(anOrientation)
	, myAlpha(0.f)
	, myIsPlayer(false)
	, myShouldUpdate(true)
{
	mySendTime = 1 * 0.033f;
	myFirstPosition = myOrientation.GetPos();
	mySecondPosition = { 1.f, 0.5f, -56.f };
	mySecondPosition2 = { 0.f, 0.f, 0.f };
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_SET_POSITION, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_HIT, this);

}


NetworkComponent::~NetworkComponent()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_SET_POSITION, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_HIT, this);

}

void NetworkComponent::Reset()
{
	myShouldUpdate = true;
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
		myCurrentRotationY = CU::Math::Lerp(myPrevRotationY, myServerRotationY, myAlpha);


		CU::Quaternion pitch = CU::Quaternion(CU::Vector3<float>(1.f, 0, 0), 0);
		CU::Quaternion yaw = CU::Quaternion(CU::Vector3<float>(0, 1.f, 0), myCurrentRotationY);

		CU::Vector3<float> axisX(1.f, 0, 0);
		CU::Vector3<float> axisY(0, 1.f, 0);
		CU::Vector3<float> axisZ(0, 0, 1.f);

		axisX = yaw * pitch * axisX;
		axisY = yaw * pitch * axisY;
		axisZ = yaw * pitch * axisZ;

		myOrientation.myMatrix[0] = axisX.x;
		myOrientation.myMatrix[1] = axisX.y;
		myOrientation.myMatrix[2] = axisX.z;
		myOrientation.myMatrix[4] = axisY.x;
		myOrientation.myMatrix[5] = axisY.y;
		myOrientation.myMatrix[6] = axisY.z;
		myOrientation.myMatrix[8] = axisZ.x;
		myOrientation.myMatrix[9] = axisZ.y;
		myOrientation.myMatrix[10] = axisZ.z;

		myOrientation.SetPos(newPos);
	}
	else
	{
		if (myShouldUpdate == false)
		{
			return;
		}
		mySendTime -= aDelta;
		if (mySendTime < 0.f)
		{
			//myServerPosition += aDelta * 5.f;
			if (myIsPlayer == false)
			{
				if (myEntity.GetComponent<HealthComponent>()->GetCurrentHealth() > 0)
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
					PostMaster::GetInstance()->SendMessage(NetworkOnDeathMessage(myNetworkID));
					myShouldUpdate = false;
					return;
				}
			}

			mySendTime = 1 * 0.033f;
			PostMaster::GetInstance()->SendMessage(NetworkSendPositionMessage(myServerPosition, 0.f, myNetworkID));
		}
	}
}

void NetworkComponent::ReceiveMessage(const NetworkSetPositionMessage& aMessage)
{
	if (aMessage.myNetworkID == myNetworkID)
	{
		myPrevPosition = myServerPosition;
		myServerPosition = aMessage.myPosition;
		myPrevRotationY = myServerRotationY;
		myServerRotationY = aMessage.myRotationY;
		myAlpha = 0.f;
	}
}

void NetworkComponent::ReceiveMessage(const NetworkOnHitMessage& aMessage)
{
	if (myNetworkID == aMessage.myNetworkID)
	{
		myEntity.SendNote(DamageNote(static_cast<int>(aMessage.myDamage)));
		
	}
}

void NetworkComponent::SetPlayer(bool aBool)
{
	myIsPlayer = aBool;
}
