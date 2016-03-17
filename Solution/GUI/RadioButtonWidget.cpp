#include "stdafx.h"
#include <AudioInterface.h>
#include "RadioButtonContainer.h"
#include "RadioButtonWidget.h"
#include <CommonHelper.h>
#include <Engine.h>
#include <PostMaster.h>

namespace GUI
{
	RadioButtonWidget::RadioButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, eOnRadioButtonEvent aRadioButtonType, RadioButtonContainer& aContainer)
		: Widget()
		, myContainer(aContainer)
		, myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
		, myHoverText("")
		, myIsTextButton(false)
		, myId(-1)
		, myActive(false)
		, myType(aRadioButtonType)
	{
		std::string spritePathNormal = "";
		std::string spritePathHover = "";
		std::string spritePathPressed = "";
		std::string spritePathActive = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		tinyxml2::XMLElement* textElement = aReader->FindFirstChild(anXMLElement, "text");

		if (textElement != nullptr)
		{
			myIsTextButton = true;
			aReader->ForceReadAttribute(textElement, "value", myButtonText);

			if (CU::ToLower(myButtonText) == "default")
			{
				myIsVisible = false;
			}
		}
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritenormal"), "path", spritePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritehover"), "path", spritePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritepressed"), "path", spritePathPressed);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spriteactive"), "path", spritePathActive);

		myImageNormal = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathNormal, mySize, mySize / 2.f);
		myImageHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathHover, mySize, mySize / 2.f);
		myImagePressed = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathPressed, mySize, mySize / 2.f);
		myImageActive = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathActive, mySize, mySize / 2.f);
		myImageCurrent = myImageNormal;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "id"), "value", myId);

		tinyxml2::XMLElement* hoverElement = aReader->FindFirstChild(anXMLElement, "hover");
		if (hoverElement != nullptr)
		{
			aReader->ForceReadAttribute(hoverElement, "text", myHoverText);
		}
	}

	RadioButtonWidget::~RadioButtonWidget()
	{
		SAFE_DELETE(myImageNormal);
		SAFE_DELETE(myImagePressed);
		SAFE_DELETE(myImageHover);
		SAFE_DELETE(myImageActive);
		myImageCurrent = nullptr;
	}

	void RadioButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsVisible == true)
		{
			if (myActive == true)
			{
				myImageActive->Render(myPosition + aParentPosition);
			}
			else
			{
				myImageCurrent->Render(myPosition + aParentPosition);
			}

			if (myIsTextButton == true)
			{
				Prism::Engine::GetInstance()->PrintText(myButtonText, { aParentPosition.x + myPosition.x - mySize.x * 0.5f + 10.f, aParentPosition.y + myPosition.y - 10.f }, Prism::eTextType::RELEASE_TEXT);
			}

			if (myImageCurrent == myImageHover && myHoverText != "")
			{
				CU::Vector2<float> hoverPosition = { myPosition.x + mySize.x / 2.f, myPosition.y };
				hoverPosition += aParentPosition;

				Prism::Engine::GetInstance()->PrintText(myHoverText, hoverPosition, Prism::eTextType::RELEASE_TEXT);
			}
		}
	}

	void RadioButtonWidget::OnLeftMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImagePressed;
	}

	void RadioButtonWidget::OnLeftMouseUp()
	{
		Click();
		myImageCurrent = myImageNormal;
	}

	void RadioButtonWidget::OnMouseEnter()
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("buttonHover", 0);
		myImageCurrent = myImageHover;
	}

	void RadioButtonWidget::OnMouseExit()
	{
		myImageCurrent = myImageNormal;
	}

	void RadioButtonWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myImageNormal->SetSize(mySize, mySize / 2.f);
		myImagePressed->SetSize(mySize, mySize / 2.f);
		myImageHover->SetSize(mySize, mySize / 2.f);
		myImageActive->SetSize(mySize, mySize / 2.f);
	}

	bool RadioButtonWidget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return aPosition.x >= myPosition.x - myImageCurrent->GetHotspot().x &&
			aPosition.x <= myPosition.x + mySize.x - myImageCurrent->GetHotspot().x &&
			aPosition.y >= myPosition.y - myImageCurrent->GetHotspot().y &&
			aPosition.y <= myPosition.y + mySize.y - myImageCurrent->GetHotspot().y;
	}

	void RadioButtonWidget::SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot)
	{
		if (aIsHotspot == true)
		{
			myPosition = { aPosition.x + myImageCurrent->GetHotspot().x, aPosition.y - myImageCurrent->GetHotspot().y };
		}
		else
		{
			myPosition = aPosition;
		}
	}

	void RadioButtonWidget::SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut)
	{
		if (myId == aButtonId)
		{
			DL_ASSERT_EXP(aSuccessOut == false, CU::Concatenate("Button %d, %s duplicate.", aButtonId, aText.c_str()));
			myButtonText = aText;
			aSuccessOut = true;
			myIsVisible = true;
		}
	}

	void RadioButtonWidget::Deactivate()
	{
		myActive = false;
	}

	void RadioButtonWidget::Activate()
	{
		myActive = true;
		PostMaster::GetInstance()->SendMessage(OnRadioButtonMessage(myType, myId));
	}

	void RadioButtonWidget::Click()
	{
		myContainer.Activate(this);
	}
}