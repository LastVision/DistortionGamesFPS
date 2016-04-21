#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include "ClientNetworkManager.h"
#include <ModelLoader.h>
#include <NetMessageText.h>
#include <PostMaster.h>
#include <PrintTextMessage.h>
#include "TextEventManager.h"
#include <TextProxy.h>

#include <InputWrapper.h>
TextEventManager::TextEventManager(const Prism::Camera* aCamera)
	: myCamera(aCamera)
	, myTextStartFadingTime(1.f)
{
	myNotifications.Init(8);
	myStartOffset = { -0.596469f, 1.f, -2.f };
	for (int i = 0; i < myNotifications.GetCapacity(); i++)
	{
		NotificationText* notification = new NotificationText;
		notification->my3dText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true, true);
		notification->my3dText->Rotate3dText(-2.338453f);
		notification->my3dText->SetOffset(myStartOffset);
		notification->myCurrentText = "";
		notification->myRemainingText = "";
		notification->myIsActive = false;
		notification->myColor = { 1.f, 1.f, 1.f, 1.f };
		notification->myLifeTime = 0.f;
		myNotifications.Add(notification);
	}
	myMissionOffset = myStartOffset;
	myMissionOffset.x = 0.f;
	myMissionOffset.z += 0.5f;
	myMissionOffset.y -= 0.1f;
	myMissionText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true, true);
	myMissionText->Rotate3dText(-0.8f);
	myMissionText->SetOffset(myMissionOffset);
	myMissionText->SetText("");

	myRespawnOffset = myStartOffset;
	myRespawnOffset.x = 0.f;
	myRespawnOffset.z += 0.5f;
	myRespawnOffset.y -= 0.3f;
	myRespawnText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true, true);
	myRespawnText->Rotate3dText(-0.8f);
	myRespawnText->SetOffset(myRespawnOffset);
	myRespawnText->SetText("");


	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::TEXT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::PRINT_TEXT, this);



	for (int i = 0; i < 8; ++i)
	{
		myHasStoppedSound[i] = false;
	}
}

TextEventManager::~TextEventManager()
{
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::TEXT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::PRINT_TEXT, this);

	for (int i = 0; i < myNotifications.Size(); ++i)
	{
		std::string eventName("Stop_Notification" + std::to_string(i));
		Prism::Audio::AudioInterface::GetInstance()->PostEvent(eventName.c_str(), 0);
		myHasStoppedSound[i] = true;
	}
	for (int i = 0; i < myNotifications.Size(); ++i)
	{
		SAFE_DELETE(myNotifications[i]->my3dText);
	}
	myNotifications.DeleteAll();
	SAFE_DELETE(myMissionText);

	
}

void TextEventManager::Update(float aDeltaTime)
{
	if (myShouldRenderRespawn == true)
	{
		myRespawnTimer += aDeltaTime;
		myRespawnColor.w = abs(sin(myRespawnTimer));
		myRespawnText->SetColor(myRespawnColor);
	}

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
					
					myNotifications[i]->my3dText->SetText(myNotifications[i]->myCurrentText);
				}
				if (myNotifications[i]->myRemainingText.size() == 0 && myHasStoppedSound[i] == false)
				{
					std::string eventName("Stop_Notification" + std::to_string(i));
					Prism::Audio::AudioInterface::GetInstance()->PostEvent(eventName.c_str(), 0);
					myHasStoppedSound[i] = true;
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
	//CU::Vector2<float> position;
	//position.x = 100.f;
	//position.y = Prism::Engine::GetInstance()->GetWindowSize().y - 100.f;

	CU::Vector3<float> pos3d(myStartOffset);

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			//Prism::Engine::GetInstance()->PrintText(myNotifications[i]->myCurrentText, position, Prism::eTextType::RELEASE_TEXT, 1.f, myNotifications[i]->myColor);
			//position.y -= 20.f;

			myNotifications[i]->my3dText->SetOffset(pos3d);
			myNotifications[i]->my3dText->SetColor(myNotifications[i]->myColor);
			myNotifications[i]->my3dText->Render(myCamera);

			pos3d.y -= 0.3f * (myNotifications[i]->myTextRows + 1);
		}
	}

	if (myShouldRender == true)
	{
		myMissionText->Render(myCamera);
	}

	if (myShouldRenderRespawn == true)
	{
		myRespawnText->Render(myCamera);
	}
}

void TextEventManager::AddNotification(std::string aText, float aLifeTime, CU::Vector4<float> aColor, int aNumberOfRows)
{
	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == false)
		{
			myNotifications[i]->my3dText->SetText("");
			myNotifications[i]->myRemainingText = aText;
			myNotifications[i]->myCurrentText = "";
			myNotifications[i]->myLifeTime = aLifeTime;
			myNotifications[i]->myColor = aColor;
			myNotifications[i]->myIsActive = true;
			myNotifications[i]->myCurrentLetterInterval = 0.f;
			myNotifications[i]->myNextLetterInterval = (aLifeTime - myTextStartFadingTime) / aText.size();
			myNotifications[i]->myNextLetterInterval = fmin(myNotifications[i]->myNextLetterInterval, 0.05f);
			myNotifications[i]->myTextRows = aNumberOfRows;

			std::string eventName("Play_Notification" + std::to_string(i));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent(eventName.c_str(), 0);
			myHasStoppedSound[i] = false;
			return;
		}
	}
}

void TextEventManager::AddRespawnText(std::string aText, bool aShouldShow, CU::Vector4<float> aColor)
{
	myRespawnText->SetText(aText);
	myRespawnText->SetColor(aColor);
	myRespawnColor = aColor;
	myShouldRenderRespawn = aShouldShow;
}

void TextEventManager::ReceiveNetworkMessage(const NetMessageText& aMessage, const sockaddr_in&)
{
	if (aMessage.myText.size() > 0 && aMessage.myIsMissionText == false)
	{
		AddNotification(aMessage.myText, aMessage.myTime, aMessage.myColor, aMessage.myTextRows);
	}
	else if (aMessage.myIsMissionText == true)
	{
		myMissionText->SetText(aMessage.myText);
		myShouldRender = aMessage.myShouldShow;
	}

	if (aMessage.mySoundEvent != "")
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent(aMessage.mySoundEvent.c_str(), 0);
	}
}

void TextEventManager::ReceiveMessage(const PrintTextMessage& aMessage)
{
	AddNotification(aMessage.myText, aMessage.myTime, aMessage.myColor, aMessage.myTextRows);
}