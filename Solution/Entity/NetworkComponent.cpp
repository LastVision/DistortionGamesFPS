#include "stdafx.h"
#include "NetworkComponent.h"
#include "Entity.h"
#include <MathHelper.h>

#include <PostMasterNetSetPositionMessage.h>
#include <PostMasterNetSendPositionMessage.h>
#include <PostMasterNetOnHitMessage.h>
#include <PostMasterNetOnDeathMessage.h>
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
	mySendTime = NETWORK_UPDATE_INTERVAL;
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

void NetworkComponent::Update(float aDelta)
{
	if (myEntity.GetIsClient() == true)
	{
		ClientUpdate(aDelta);
	}
	else
	{
		ServerUpdate(aDelta);
	}
}

void NetworkComponent::ReceiveMessage(const PostMasterNetSetPositionMessage& aMessage)
{
	if (aMessage.myGID == myEntity.GetGID())
	{
		myPrevPosition = myServerPosition;
		myServerPosition = aMessage.myPosition;
		myPrevRotationY = myServerRotationY;
		myServerRotationY = aMessage.myRotationY;
		myAlpha = 0.f;
	}
}

void NetworkComponent::ReceiveMessage(const PostMasterNetOnHitMessage& aMessage)
{
	if (myEntity.GetIsClient() == false && myEntity.GetGID() == aMessage.myGID)
	{
		myEntity.SendNote(DamageNote(static_cast<int>(aMessage.myDamage)));
	}
}

void NetworkComponent::SetPlayer(bool aBool)
{
	myIsPlayer = aBool;
}


void NetworkComponent::ClientUpdate(float aDelta)
{
	myAlpha += aDelta * NETWORK_UPDATE_INTERVAL;
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

void NetworkComponent::ServerUpdate(float aDelta)
{
	myAlpha += aDelta * 0.25f;
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
				PostMaster::GetInstance()->SendMessage(PostMasterNetOnDeathMessage(myEntity.GetGID()));
				myShouldUpdate = false;
				return;
			}
		}
		mySendTime = NETWORK_UPDATE_INTERVAL;
		PostMaster::GetInstance()->SendMessage(PostMasterNetSendPositionMessage(myServerPosition, myServerRotationY, myEntity.GetGID()));

	}
}