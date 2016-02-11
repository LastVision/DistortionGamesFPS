#include "stdafx.h"
#include <AudioInterface.h>
#include <Cursor.h>
#include <InputWrapper.h>
#include "LoadingState.h"
#include <Sprite.h>
#include <SpriteProxy.h>
#include <Text.h>
#include <GUIManager.h>


#include <FadeMessage.h>
#include <PostMaster.h>

LoadingState::LoadingState()
	: myRotatingThingScale(0.f)
	, myGUIManager(nullptr)
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myRotatingThing = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loadingscreen_rotating_thing.dds"
		, { 256.f, 256.f }, { 128.f, 128.f });
	myRotatingThing2 = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loadingscreen_rotating_thing_2.dds"
		, { 256.f, 256.f }, { 128.f, 128.f });

	myFinishedTextAlpha = 0.f;
	myFinishedTextFadeIn = false;
	myIsDone = false;
}


LoadingState::~LoadingState()
{
	SAFE_DELETE(myRotatingThing);
	SAFE_DELETE(myRotatingThing2);
	SAFE_DELETE(myGUIManager);
}

void LoadingState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myCursor = aCursor;
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_loading_screen.xml", nullptr, nullptr, nullptr, -1);
	OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Menu", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Loading", 0);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LoadingState::EndState()
{
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 30.f));
	myCursor->SetShouldRender(true);
}

const eStateStatus LoadingState::Update(const float& aDeltaTime)
{
	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
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
	}

	float speed1 = -3.f;
	float speed2 = 0.0f;

	myRotatingThing->Rotate(speed1 * aDeltaTime * (1.f / (myRotatingThingScale + 0.001f)));
	myRotatingThing2->Rotate(speed2 * aDeltaTime * (1.f / (myRotatingThingScale + 0.001f)));

	if (myRotatingThingScale < 1.f)
	{
		myRotatingThing->Rotate(speed1 * aDeltaTime);
		myRotatingThing2->Rotate(speed2 * aDeltaTime);
	}

	if (Prism::ModelLoader::GetInstance()->IsLoading() == true)
	{
		myRotatingThingScale += aDeltaTime * 2.f;

		if (myRotatingThingScale > 1.f)
		{
			myRotatingThingScale = 1.f;
		}
	}
	else
	{
		myRotatingThingScale -= aDeltaTime * 2.f;

		if (myRotatingThingScale < 0.f)
		{
			myRotatingThingScale = 0.f;
		}

		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true
			|| CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(0) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(1) == true
			|| CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true)
		{
			return eStateStatus::ePopSubState;
		}
	}

	return eStateStatus::eKeepState;
	//if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	//{
	//	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RETURN))
	//	{

	//		return eStateStatus::ePopSubState;
	//	}
	//	return eStateStatus::eKeepState;
	//}

	//return eStateStatus::eKeepState;
}

void LoadingState::Render()
{
	myGUIManager->Render();

	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));
	//myBackground->Render(windowSize * 0.5f);

	//myText->Render();

	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
		Prism::Engine::GetInstance()->PrintText("Press [space] to begin."
			, { windowSize.x * 0.5f - 150.f, windowSize.y  * 0.5f - 350.f }, Prism::eTextType::RELEASE_TEXT
			, 1.f, { 1.f, 1.f, 1.f, myFinishedTextAlpha });
	}

	CU::Vector2<float> position = { windowSize.x * 0.5f + 400.f, windowSize.y *0.5f - 350.f };
	CU::Vector2<float> scale = { myRotatingThingScale * 0.5f, myRotatingThingScale * 0.5f };

	myRotatingThing->Render(position, scale);
	myRotatingThing2->Render(position, scale);
}

void LoadingState::ResumeState()
{
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LoadingState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myGUIManager->OnResize(aWidth, aHeight);
}

