#include "stdafx.h"
#include <AudioInterface.h>
#include "OptionsState.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "PostMaster.h"
#include "OnClickMessage.h"
#include "InGameState.h"
#include "HelpState.h"
#include <TextProxy.h>
#include <WidgetContainer.h>
#include <ModelLoader.h>
#include <FadeMessage.h>
OptionsState::OptionsState()
	: myGUIManager(nullptr)
{
}


OptionsState::~OptionsState()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myMusicText);
	SAFE_DELETE(mySfxText);
	SAFE_DELETE(myVoiceText);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void OptionsState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_options_menu.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();

	myMusicText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	mySfxText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	myVoiceText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	OnResize(windowSize.x, windowSize.y);

	myMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
	mySfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
	myVoiceVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();


	myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
	mySfxText->SetText("SFX: " + std::to_string(mySfxVolume));
	myVoiceText->SetText("Voice: " + std::to_string(myVoiceVolume));

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void OptionsState::EndState()
{
	myIsActiveState = false;
}

void OptionsState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
	bool notDone = true;
	CU::Vector2<float> floatScreenPos(float(aWidth), float(aHeight));
	GUI::WidgetContainer* widgetCont = reinterpret_cast<GUI::WidgetContainer*>(myGUIManager->GetWidgetContainer()->At(0));
	CU::Vector2<float> firstRow = widgetCont->At(3)->GetPosition();
	CU::Vector2<float> secondRow = widgetCont->At(1)->GetPosition();
	CU::Vector2<float> thirdRow = widgetCont->At(5)->GetPosition();
	CU::Vector2<float> fourthRow = widgetCont->At(7)->GetPosition();
	float moveAmount = 320.f; 
	float ratio = static_cast<float>(aWidth) / static_cast<float>(aHeight);

	float epsilon = 0.1f;
	if (ratio + epsilon >= 16.f / 9.f)
	{
		moveAmount = 320.f;
	}
	else if (ratio + epsilon >= 16.f / 10.f)
	{
		moveAmount = 320.f;
	}
	else
	{
		moveAmount = 220.f;
	}
	myMusicText->SetPosition({ firstRow.x - moveAmount, firstRow.y - (widgetCont->At(1)->GetSize().y * 0.25f) });
	mySfxText->SetPosition({ secondRow.x - moveAmount, secondRow.y - (widgetCont->At(3)->GetSize().y * 0.25f) });
	myVoiceText->SetPosition({ thirdRow.x - moveAmount, thirdRow.y - (widgetCont->At(5)->GetSize().y * 0.2f) });
}

const eStateStatus OptionsState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopSubState;
	}

	int currentMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
	if (currentMusicVolume != myMusicVolume)
	{
		myMusicVolume = currentMusicVolume;
		myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
	}

	int currentSfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
	if (currentSfxVolume != mySfxVolume)
	{
		mySfxVolume = currentSfxVolume;
		mySfxText->SetText("SFX: " + std::to_string(mySfxVolume));
	}

	int currentVoiceVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
	if (currentVoiceVolume != myVoiceVolume)
	{
		myVoiceVolume = currentVoiceVolume;
		myVoiceText->SetText("Voice: " + std::to_string(myVoiceVolume));
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void OptionsState::Render()
{
	myGUIManager->Render();
	myMusicText->Render();
	mySfxText->Render();
	myVoiceText->Render();
}

void OptionsState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void OptionsState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
	case eOnClickEvent::INCREASE_VOLUME:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVolume", 0);
		break;
	case eOnClickEvent::LOWER_VOLUME:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVolume", 0);
		break;
	case eOnClickEvent::INCREASE_MUSIC:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseMusic", 0);
		break;
	case eOnClickEvent::LOWER_MUSIC:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerMusic", 0);
		break;
	case eOnClickEvent::INCREASE_VOICE:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVoice", 0);
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Not_Enough_Fuel", 0);
		break;
	case eOnClickEvent::LOWER_VOICE:
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVoice", 0);
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Not_Enough_Fuel", 0);
		break;
		case eOnClickEvent::RESUME_GAME:
			myIsActiveState = false;
			myStateStatus = eStateStatus::ePopSubState;
			break;
		default:
			break;
		}
	}
}
