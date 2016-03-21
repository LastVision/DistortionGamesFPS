#include "stdafx.h"
#include "NetworkComponent.h"
#include "Entity.h"
#include <MathHelper.h>

#include <NetMessageOnDeath.h>
#include <NetMessageOnHit.h>
#include <NetMessagePosition.h>

#include "DamageNote.h"

#include "HealthComponent.h"
#include <PostMaster.h>
#include <Quaternion.h>
#include <SharedNetworkManager.h>
#include "PollingStation.h"

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
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::POSITION, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_HIT, this);
}


NetworkComponent::~NetworkComponent()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::POSITION, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_HIT, this);
}

void NetworkComponent::Reset()
{
	myShouldUpdate = true;
}

void NetworkComponent::Update(float aDelta)
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
	//myOrientation.CreateRotateAroundY(10*aDelta);
	myOrientation.SetPos(newPos);
}

void NetworkComponent::ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress)
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

void NetworkComponent::ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress)
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

// legacy, remove when we have AI on server
//void NetworkComponent::ServerUpdate(float aDelta)
//{
//	myAlpha += aDelta * 0.25f;
//	if (myShouldUpdate == false)
//	{
//		return;
//	}
//	mySendTime -= aDelta;
//	if (mySendTime < 0.f)
//	{
//		//myServerPosition += aDelta * 5.f;
//		if (myIsPlayer == false)
//		{
//			if (myEntity.GetComponent<HealthComponent>()->GetCurrentHealth() > 0)
//			{
//				if (myShouldReturn == false)
//				{
//					if (myFirstPosition.z < 0.f)
//					{
//						myServerPosition = CU::Math::Lerp(myFirstPosition, mySecondPosition2, myAlpha);
//					}
//					else
//					{
//						myServerPosition = CU::Math::Lerp(myFirstPosition, mySecondPosition, myAlpha);
//					}
//				}
//				else
//				{
//					if (myFirstPosition.z < 0.f)
//					{
//						myServerPosition = CU::Math::Lerp(mySecondPosition2, myFirstPosition, myAlpha);
//					}
//					else
//					{
//						myServerPosition = CU::Math::Lerp(mySecondPosition, myFirstPosition, myAlpha);
//					}
//				}
//				if (myAlpha >= 1)
//				{
//					myShouldReturn = !myShouldReturn;
//					myAlpha = 0;
//				}
//			}
//			else
//			{
//				SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnDeath(eNetMessageType::ENEMY_ON_DEATH,myEntity.GetGID()));
//				myShouldUpdate = false;
//				return;
//			}
//		}
//		mySendTime = NETWORK_UPDATE_INTERVAL;
//		SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(myServerPosition, myServerRotationY, myEntity.GetGID()));
//	}
//}