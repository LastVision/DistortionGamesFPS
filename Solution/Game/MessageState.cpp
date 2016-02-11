#include "stdafx.h"
#include <Camera.h>
#include <Engine.h>
#include "FadeMessage.h"
#include "GameStateMessage.h"
#include "MessageState.h"
#include <ModelLoader.h>
#include <SpriteProxy.h>
#include <InputWrapper.h>
#include "PostMaster.h"

MessageState::MessageState(const std::string& aTexturePath, const CU::Vector2<float>& aSize)
	: myEvent(nullptr)
	, myTotalTime(0.f)
	, myFinishedTextAlpha(0.f)
	, myFinishedTextFadeIn(true)
{
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(aTexturePath, aSize, aSize / 2.f);
	/*myBackground = new Prism::Sprite(aTexturePath, aSize, aSize / 2.f);*/
	myTextMessage = "";
	mySpriteSize = { 64.f, 64.f };

	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myBlackOverlay = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/Splash/T_background_default.dds", windowSize, windowSize / 2.f);
	//myBlackOverlay = new Prism::Sprite("Data/Resource/Texture/Menu/Splash/T_background_default.dds", windowSize, windowSize / 2.f);
}

MessageState::~MessageState()
{
	delete myBackground;
	delete myEvent;
	delete myBlackOverlay;
	myBackground = nullptr;
	myEvent = nullptr;
	myBlackOverlay = nullptr;
}

void MessageState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = true;
	myStateStack = aStateStackProxy;
	CU::Matrix44<float> orientation;
	myCursor = aCursor;
	//myCamera = new Prism::Camera(orientation);

	OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void MessageState::EndState()
{
	
}

const eStateStatus MessageState::Update(const float& aDeltaTime)
{
	myTotalTime += aDeltaTime;
	if (myTotalTime < 4.f)
	{
		return eStateStatus::eKeepState;
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true
		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
		|| CU::InputWrapper::GetInstance()->MouseDown(0) == true
		|| CU::InputWrapper::GetInstance()->MouseDown(1) == true
		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true)
	{
		if (myEvent != nullptr)
		{
			PostMaster::GetInstance()->SendMessage(*myEvent);
		}
		return eStateStatus::ePopSubState;
	}


	if (myFinishedTextFadeIn == false)
	{
		myFinishedTextAlpha -= aDeltaTime;
		if (myFinishedTextAlpha <= 0.f)
		{
			myFinishedTextAlpha = 0.f;
			myFinishedTextFadeIn = true;
		}
	}
	else
	{
		myFinishedTextAlpha += aDeltaTime;
		if (myFinishedTextAlpha >= 1.f)
		{
			myFinishedTextAlpha = 1.f;
			myFinishedTextFadeIn = false;
		}
	}

	return eStateStatus::eKeepState;
}

void MessageState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x / 2.f)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y / 2.f));

	myBlackOverlay->Render(windowSize, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 0.5f });

	myBackground->Render(windowSize);

	if (myTotalTime > 4.f && myTextMessage != "")
	{
	/*	Prism::Engine::GetInstance()->PrintText(myTextMessage, { myMessagePosition.x + 342.f, myMessagePosition.y + 340.f }, Prism::eTextType::RELEASE_TEXT
			, 1.f, { 1.f, 1.f, 1.f, myFinishedTextAlpha });
*/
		CU::Vector2<float> windowSize2 = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
			float(Prism::Engine::GetInstance()->GetWindowSize().y));
		Prism::Engine::GetInstance()->PrintText(myTextMessage
			, { windowSize2.x * 0.5f - 150.f, windowSize2.y  * 0.5f - 350.f }, Prism::eTextType::RELEASE_TEXT
			, 1.f, { 1.f, 1.f, 1.f, myFinishedTextAlpha });
	}
}

void MessageState::ResumeState()
{

}

void MessageState::OnResize(int, int)
{
	//myCamera->OnResize(aWidth, aHeight);

	myMessagePosition.x = (Prism::Engine::GetInstance()->GetWindowSize().x / 2.f) - myBackground->GetSize().x / 2;
	myMessagePosition.y = (-Prism::Engine::GetInstance()->GetWindowSize().y / 2.f) + myBackground->GetSize().y / 2;
}
