#include "stdafx.h"
#include <Camera.h>
#include "ClientNetworkManager.h"
#include <NetMessageText.h>
#include "TextEventManager.h"
#include <Text.h>

TextEventManager::TextEventManager(const Prism::Camera* aCamera)
	: myCamera(aCamera)
	, myTextStartFadingTime(2.f)
{
	myNotifications.Init(8);
	myMissionTexts.Init(8);

	for (int i = 0; i < myNotifications.GetCapacity(); i++)
	{
		NotificationText* notification = new NotificationText;
		notification->myText = "";
		notification->myIsActive = false;
		notification->myColor = { 1.f, 1.f, 1.f, 1.f };
		notification->myLifeTime = 0.f;
		myNotifications.Add(notification);
	}

	for (int i = 0; i < myMissionTexts.GetCapacity(); i++)
	{
		MissionText* missionText = new MissionText;
		missionText->myText = "";
		missionText->myIsActive = false;
		missionText->myColor = { 1.f, 1.f, 1.f, 1.f };
		myMissionTexts.Add(missionText);
	}

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::TEXT, this);
}

TextEventManager::~TextEventManager()
{
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::TEXT, this);
	myNotifications.DeleteAll();
	myMissionTexts.DeleteAll();
}

void TextEventManager::Update(float aDeltaTime)
{
	for (int i = 0; i < myNotifications.Size(); ++i)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			myNotifications[i]->myLifeTime -= aDeltaTime;

			if (myNotifications[i]->myLifeTime <= 0.f)
			{
				myNotifications[i]->myIsActive = false;
			}
			else if (myNotifications[i]->myLifeTime <= myTextStartFadingTime)
			{
				myNotifications[i]->myColor.w = 1.f - (myTextStartFadingTime - myNotifications[i]->myLifeTime);
			}
		}
	}
}

void TextEventManager::Render()
{
	CU::Vector2<float> position = { 100.f, 900.f };

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			Prism::Engine::GetInstance()->PrintText(myNotifications[i]->myText, position, Prism::eTextType::RELEASE_TEXT, 1.f, myNotifications[i]->myColor);
			position.y -= 20.f;
		}
	}
	
	position = { 1400.f, 900.f };

	for (int i = 0; i < myMissionTexts.Size(); i++)
	{
		if (myMissionTexts[i]->myIsActive == true)
		{
			Prism::Engine::GetInstance()->PrintText(myMissionTexts[i]->myText, position, Prism::eTextType::RELEASE_TEXT, 1.f, myMissionTexts[i]->myColor);
			position.y -= 20.f;
		}
	}
}

void TextEventManager::AddNotification(std::string aText, float aLifeTime, CU::Vector4<float> aColor)
{
	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == false)
		{
			myNotifications[i]->myText = aText;
			myNotifications[i]->myLifeTime = aLifeTime;
			myNotifications[i]->myColor = aColor;
			myNotifications[i]->myIsActive = true;
			return;
		}
	}
}

void TextEventManager::ReceiveNetworkMessage(const NetMessageText& aMessage, const sockaddr_in&)
{
	AddNotification(aMessage.myText, aMessage.myTime);
}