#include "stdafx.h"
#include "CreditMenuState.h"
#include "PostMaster.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "OnClickMessage.h"
#include "InGameState.h"
#include <FadeMessage.h>
CreditMenuState::CreditMenuState()
	: myGUIManager(nullptr)
{
}


CreditMenuState::~CreditMenuState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void CreditMenuState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_credit_screen.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void CreditMenuState::EndState()
{

}

void CreditMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus CreditMenuState::Update(const float& aDeltaTime)
{
	aDeltaTime;
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();
	if (input->KeyDown(DIK_ESCAPE) == true || input->KeyDown(DIK_SPACE) == true || input->KeyDown(DIK_RETURN) == true
		|| input->MouseUp(0) == true || input->MouseUp(1) == true)
	{
		PostMaster::GetInstance()->SendMessage(OnClickMessage(eOnClickEvent::GAME_QUIT));
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void CreditMenuState::Render()
{
	myGUIManager->Render();
}

void CreditMenuState::ResumeState()
{
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void CreditMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		/*case eOnClickEvent::GAME_START:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new InGameState(aMessage.myID));
			break;*/
		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopSubState;
			break;
		default:
			DL_ASSERT("Unknown event type.");
			break;
		}
	}
}