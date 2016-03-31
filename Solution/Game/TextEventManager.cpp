#include "stdafx.h"
#include <Camera.h>
#include "ClientNetworkManager.h"
#include <NetMessageText.h>
#include "TextEventManager.h"
#include <Text.h>

TextEventManager::TextEventManager(const Prism::Camera* aCamera)
	: myCamera(aCamera)
	, myTextStartFadingTime(1.f)
{
	myNotifications.Init(8);

	for (int i = 0; i < myNotifications.GetCapacity(); i++)
	{
		NotificationText* notification = new NotificationText;
		notification->myCurrentText = "";
		notification->myRemainingText = "";
		notification->myIsActive = false;
		notification->myColor = { 1.f, 1.f, 1.f, 1.f };
		notification->myLifeTime = 0.f;
		myNotifications.Add(notification);
	}

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::TEXT, this);
}

TextEventManager::~TextEventManager()
{
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::TEXT, this);
	myNotifications.DeleteAll();
}

void TextEventManager::Update(float aDeltaTime)
{
	for (int i = 0; i < myNotifications.Size(); ++i)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			myNotifications[i]->myLifeTime -= aDeltaTime;

			if (myNotifications[i]->myRemainingText.size() > 0)
			{
				myNotifications[i]->myCurrentLetterInterval -= aDeltaTime;

				if (myNotifications[i]->myCurrentLetterInterval <= 0.f)
				{
					myNotifications[i]->myCurrentText += myNotifications[i]->myRemainingText[0];
					myNotifications[i]->myRemainingText.erase(myNotifications[i]->myRemainingText.begin());
					myNotifications[i]->myCurrentLetterInterval = myNotifications[i]->myNextLetterInterval;
				}
			}

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

	CU::Vector2<float> position;
	position.x = 100.f;
	position.y = Prism::Engine::GetInstance()->GetWindowSize().y - 100.f;

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			Prism::Engine::GetInstance()->PrintText(myNotifications[i]->myCurrentText, position, Prism::eTextType::RELEASE_TEXT, 1.f, myNotifications[i]->myColor);
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
			myNotifications[i]->myRemainingText = aText;
			myNotifications[i]->myCurrentText = "";
			myNotifications[i]->myLifeTime = aLifeTime;
			myNotifications[i]->myColor = aColor;
			myNotifications[i]->myIsActive = true;
			myNotifications[i]->myCurrentLetterInterval = 0.f;
			myNotifications[i]->myNextLetterInterval = (aLifeTime - myTextStartFadingTime) / aText.size();
			myNotifications[i]->myNextLetterInterval = fmin(myNotifications[i]->myNextLetterInterval, 0.05f);
			return;
		}
	}
}

void TextEventManager::ReceiveNetworkMessage(const NetMessageText& aMessage, const sockaddr_in&)
{
	AddNotification(aMessage.myText, aMessage.myTime);
}