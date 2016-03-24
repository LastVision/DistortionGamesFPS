#include "stdafx.h"
#include <Camera.h>
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

	//myMissionTexts[0]->myIsActive = true;
	//myMissionTexts[0]->myText = "Mission";
	//
	//myNotifications[0]->myIsActive = true;
	//myNotifications[0]->myText = "Text";
	//myNotifications[0]->myLifeTime = 5.f;
}

TextEventManager::~TextEventManager()
{
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