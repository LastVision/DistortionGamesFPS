#include "stdafx.h"

#include <AudioInterface.h>
#include "SplashState.h"
#include <Sprite.h>
#include <SpriteProxy.h>
#include <Camera.h>
#include <Cursor.h>
#include <InputWrapper.h>
#include "StateStackProxy.h"
#include <TimerManager.h>
#include "GameStateMessage.h"
#include <InputWrapper.h>

SplashState::SplashState(const std::string& aTexturePath, bool aStartSound, bool aVictoryScreen)
	: myVictoryScreen(aVictoryScreen)
	, myStartSound(aStartSound)
	, myFadeOut(false)
	, myDisplay(false)
	, myLogoAlpha(0.f)
	, myFadeInTime(0.f)
	, myFadeOutTime(0.f)
	, myDisplayTime(0.f)
	, myCurrentTime(0.f)
	, myCamera(nullptr)
{
	CU::Vector2<float> size(1024.f, 1024.f);
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	if (aVictoryScreen == true)
	{
		size = { windowSize.y * 2.f, windowSize.y };
	}
	myLogo = Prism::ModelLoader::GetInstance()->LoadSprite(aTexturePath, size, size / 2.f);

	CU::Vector2<float> backgroundSize(4096.f, 4096.f);
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/Splash/T_background_default.dds", 
		windowSize, windowSize / 2.f);
}

SplashState::~SplashState()
{
}

void SplashState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	CU::Matrix44<float> orientation;
	myCamera = new Prism::Camera(orientation);
	myCursor = aCursor;

	OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);

	myLogoAlpha = 0.f;

	myFadeInTime = 1.5f;
	myFadeOutTime = 1.5f;
	myDisplayTime = 1.5f;

	if (myStartSound == true)
	{
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Menu", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MainMenu", 0);
	}

	if (myVictoryScreen == true)
	{
		myDisplayTime += 3.f;
	}

	myCurrentTime = 0;
	myFadeOut = false;
	myDisplay = false;

	myCursor->SetShouldRender(false);
}

void SplashState::EndState()
{

	if (myStartSound == false)
	{
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSplash", 0);
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Menu", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MainMenu", 0);
	}
	SAFE_DELETE(myLogo);
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myCamera);

	myCursor->SetShouldRender(true);
}

const eStateStatus SplashState::Update(const float& aDeltaTime)
{
	float deltaTime = fminf(1.f / 30.f, aDeltaTime);

	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	myCurrentTime += deltaTime;

	if (input->KeyDown(DIK_SPACE) == true
		|| input->KeyDown(DIK_ESCAPE) == true
		|| input->KeyDown(DIK_RETURN) == true
		|| input->MouseDown(0) == true
		|| input->MouseDown(1) == true
		|| input->MouseDown(2) == true)
	{
		myCurrentTime += myFadeInTime + myDisplayTime;
	}

	if (myFadeOut == true)
	{
		myLogoAlpha -= deltaTime / myFadeOutTime;
	}
	else if (myDisplay == false)
	{
		myLogoAlpha += deltaTime / myFadeInTime;
	}
	else
	{
		myLogoAlpha = 1.f;
	}

	myLogoAlpha = fmaxf(fminf(myLogoAlpha, 1.f), 0);

	if (myDisplay == false && myCurrentTime > myFadeInTime)
	{
		myDisplay = true;
	}
	if (myFadeOut == false && myCurrentTime > myFadeInTime + myDisplayTime)
	{
		myFadeOut = true;
	}

	if (myFadeOut == true && myLogoAlpha == 0)
	{
		return ePopSubState;
	}

	return eKeepState;
}

void SplashState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));
	myBackground->Render(windowSize / 2.f);
	myLogo->Render(windowSize / 2.f, { 1.f, 1.f }, { 1.f, 1.f, 1.f, myLogoAlpha });
}

void SplashState::ResumeState()
{
	myCursor->SetShouldRender(false);
}

void SplashState::OnResize(int aWidth, int aHeight)
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	if (myBackground != nullptr)
	{
		myBackground->SetSize(windowSize, windowSize / 2.f);
	}
	myCamera->OnResize(aWidth, aHeight);
}