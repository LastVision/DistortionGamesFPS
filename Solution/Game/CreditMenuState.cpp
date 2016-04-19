#include "stdafx.h"
#include <Cursor.h>
#include "CreditMenuState.h"
#include "PostMaster.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "OnClickMessage.h"
#include "InGameState.h"
#include <FadeMessage.h>
CreditMenuState::CreditMenuState(float aShowTimer)
	: myGUIManager(nullptr)
	, myShowTimer(aShowTimer)
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
	myCursor->SetShouldRender(true);
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_credit_screen.xml", nullptr, -1);

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
	myShowTimer -= aDeltaTime;
	if (myShowTimer > 0)
	{
		return myStateStatus;
	}
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();
	if (input->KeyDown(DIK_ESCAPE) == true || input->KeyDown(DIK_SPACE) == true || input->KeyDown(DIK_RETURN) == true
		|| input->MouseUp(0) == true || input->MouseUp(1) == true)
	{
		myStateStatus = eStateStatus::ePopMainState;
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
		bool notDone = true;
		switch (aMessage.myEvent)
		{
		/*case eOnClickEvent::GAME_START:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new InGameState(aMessage.myID));
			break;*/

		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopMainState;
			break;
		default:
			DL_ASSERT("Unknown event type.");
			break;
		}
	}
}