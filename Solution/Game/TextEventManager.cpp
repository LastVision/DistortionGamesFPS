#include "stdafx.h"
#include <Camera.h>
#include <InWorldTextMessage.h>
#include <ModelLoader.h>
#include <NotificationMessage.h>
#include "TextEventManager.h"
#include <Text.h>
#include <PostMaster.h>

TextEventManager::TextEventManager(const Prism::Camera* aCamera)
	: myCamera(aCamera)
	, myNotificationPosition(50.f, 900.f)
	, myTextLifeTime(5.f)
	, myTextStartFadingTime(1.f)
	, myInWorldTextIndex(0)
	, myNotificationIndex(0)
	, myNotificationMax(10)
	, myInWorldTextMax(10)
	, myTextScale(1.f)
	, mySize(1920.f, 1080.f)
{
	myNotifications.Init(myNotificationMax);
	myInWorldTexts.Init(myInWorldTextMax);

	for (int i = 0; i < myInWorldTextMax; i++)
	{
		InWorldText* inworldText = new InWorldText;
		inworldText->myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
		myInWorldTexts.Add(inworldText);
	}

	for (int i = 0; i < myNotificationMax; i++)
	{
		NotificationText* notification = new NotificationText;
		notification->myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
		myNotifications.Add(notification);
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::IN_WORLD_TEXT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NOTIFICATION, this);
}

TextEventManager::~TextEventManager()
{
	for (int i = 0; i < myInWorldTexts.Size(); i++)
	{
		SAFE_DELETE(myInWorldTexts[i]->myText);
		SAFE_DELETE(myInWorldTexts[i]);
	}

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		SAFE_DELETE(myNotifications[i]->myText);
		SAFE_DELETE(myNotifications[i]);
	}

	PostMaster::GetInstance()->UnSubscribe(eMessageType::IN_WORLD_TEXT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NOTIFICATION, this);
}

void TextEventManager::Update(float aDeltaTime)
{
	for (int i = 0; i < myInWorldTexts.Size(); ++i)
	{
		if (myInWorldTexts[i]->myIsActive == true)
		{
			myInWorldTexts[i]->myLifeTime -= aDeltaTime;
			myInWorldTexts[i]->myInWorldPosition.y += aDeltaTime;

			if (myInWorldTexts[i]->myLifeTime <= 0.f)
			{
				myInWorldTexts[i]->myIsActive = false;
			}
			else if (myInWorldTexts[i]->myLifeTime <= myTextStartFadingTime)
			{
				myInWorldTexts[i]->myColor.w = 1.f - (myTextStartFadingTime - myInWorldTexts[i]->myLifeTime);
			}
		}
	}

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
	for (int i = 0; i < myInWorldTexts.Size(); i++)
	{
		if (myInWorldTexts[i]->myIsActive == true)
		{
			myInWorldTexts[i]->myText->SetPosition(Get2DPosition(myInWorldTexts[i]->myInWorldPosition));
			myInWorldTexts[i]->myText->SetColor(myInWorldTexts[i]->myColor);
			myInWorldTexts[i]->myText->SetScale({ 1.f, 1.f });
			myInWorldTexts[i]->myText->Render();
		}
	}

	CU::Vector2<float> position = myNotificationPosition;

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			myNotifications[i]->myText->SetPosition(position);
			myNotifications[i]->myText->SetColor(myNotifications[i]->myColor);
			myNotifications[i]->myText->SetScale({ 1.f, 1.f });
			myNotifications[i]->myText->Render();
			position.y -= 20.f;
		}
	}
}

void TextEventManager::AddNotification(const std::string& aText, const CU::Vector4<float>& aColor)
{
	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
		myNotifications[myNotificationIndex]->myText->SetText(aText);
		myNotifications[myNotificationIndex]->myColor = aColor;
		myNotifications[myNotificationIndex]->myLifeTime = myTextLifeTime;
		myNotifications[myNotificationIndex]->myIsActive = true;

		myNotificationIndex++;

		if (myNotificationIndex >= myNotificationMax)
		{
			myNotificationIndex = 0;
		}
	}
}

void TextEventManager::AddInWorldText(const std::string& aText, const CU::Vector2<float>& aPosition, const CU::Vector4<float>& aColor)
{
	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
		myInWorldTexts[myInWorldTextIndex]->myText->SetText(aText);
		myInWorldTexts[myInWorldTextIndex]->myColor = aColor;
		myInWorldTexts[myInWorldTextIndex]->myLifeTime = myTextLifeTime;
		myInWorldTexts[myInWorldTextIndex]->myIsActive = true;
		myInWorldTexts[myInWorldTextIndex]->myInWorldPosition = { aPosition.x, 0.f, aPosition.y };

		myInWorldTextIndex++;

		if (myInWorldTextIndex >= myInWorldTextMax)
		{
			myInWorldTextIndex = 0;
		}
	}
}

void TextEventManager::AddInWorldText(const std::string& aText, const CU::Vector3<float>& aPosition, const CU::Vector4<float>& aColor)
{
	AddInWorldText(aText, { aPosition.x, aPosition.z }, aColor);
}

void TextEventManager::ReceiveMessage(const NotificationMessage& aMessage)
{
	AddNotification(aMessage.myText, aMessage.myColor);
}

void TextEventManager::ReceiveMessage(const InWorldTextMessage& aMessage)
{
	AddInWorldText(aMessage.myText, aMessage.myPosition, aMessage.myColor);
}

void TextEventManager::OnResize(const CU::Vector2<float>& aNewSize)
{
	myTextScale = (myTextScale / mySize.x) * aNewSize.x;

	myNotificationPosition = (myNotificationPosition / mySize) * aNewSize;




	mySize = aNewSize;
}

CU::Vector2<float> TextEventManager::Get2DPosition(const CU::Vector3<float>& aPosition)
{
	CU::Matrix44<float> renderPos;
	renderPos.SetPos(aPosition);
	renderPos = renderPos * CU::InverseSimple(myCamera->GetOrientation());
	renderPos = renderPos * myCamera->GetProjection();

	CU::Vector3<float> newRenderPos = renderPos.GetPos();
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

	newRenderPos /= renderPos.GetPos4().w;
	newRenderPos += 1.f;
	newRenderPos *= 0.5f;
	newRenderPos.x *= windowSize.x;
	newRenderPos.y *= windowSize.y;

	return{ newRenderPos.x, newRenderPos.y + 10.f };
}