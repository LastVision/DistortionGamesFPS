#include "stdafx.h"
#include "GUIManager.h"
#include "HelpState.h"
#include "InGameState.h"
#include "InputWrapper.h"
#include "OnClickMessage.h"
#include "PostMaster.h"
#include <FadeMessage.h>
HelpState::HelpState()
	: myGUIManager(nullptr)
{
}


HelpState::~HelpState()
{
	SAFE_DELETE(myGUIManager);
		myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void HelpState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_help_menu.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HelpState::EndState()
{
	myIsActiveState = false;
	Prism::Engine::GetInstance()->SetShouldRenderText(true);
}

void HelpState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus HelpState::Update(const float& aDeltaTime)
{
	aDeltaTime;


	CU::InputWrapper* input = CU::InputWrapper::GetInstance();
	if (input->KeyDown(DIK_ESCAPE) == true || input->KeyDown(DIK_SPACE) == true || input->KeyDown(DIK_RETURN) == true
		|| input->MouseUp(0) == true || input->MouseUp(1) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopSubState;
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void HelpState::Render()
{
	Prism::Engine::GetInstance()->SetShouldRenderText(false);
	myGUIManager->Render();
}

void HelpState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HelpState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::RESUME_GAME:
			myIsActiveState = false;
			myStateStatus = eStateStatus::ePopSubState;
			break;
		}
	}
}
