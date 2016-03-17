#include "stdafx.h"

#include <CommonHelper.h>
#include "RadioButtonContainer.h"
#include "RadioButtonWidget.h"
#include "SpriteWidget.h"
#include <OnRadioButtonMessage.h>

namespace GUI
{
	RadioButtonContainer::RadioButtonContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: myButtons(32)
		, myActiveIndex(0)
	{
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		std::string radioButtonTypeStr;
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "type"), "value", radioButtonTypeStr);

		eOnRadioButtonEvent radioButtonType = eOnRadioButtonEvent::NONE;
		if (radioButtonTypeStr == "levelSelect")
		{
			radioButtonType = eOnRadioButtonEvent::LEVEL_SELECT;
		}

		DL_ASSERT_EXP(radioButtonType != eOnRadioButtonEvent::NONE, CU::Concatenate("Unknown radio button type: %s", radioButtonTypeStr.c_str()));

		myBackground = new SpriteWidget(aReader, aReader->ForceFindFirstChild(anXMLElement, "background"));

		tinyxml2::XMLElement* widgetElement = aReader->FindFirstChild(anXMLElement, "widget");

		for (; widgetElement != nullptr; widgetElement = aReader->FindNextElement(widgetElement, "widget"))
		{
			std::string type = "";

			aReader->ForceReadAttribute(widgetElement, "type", type);

			if (type == "radioButton")
			{
				RadioButtonWidget* button = new RadioButtonWidget(aReader, widgetElement, radioButtonType, *this);
				myButtons.Add(button);
			}
			else
			{
				std::string message = "[RadioButtonComtainer] no widget type named " + type;
				DL_ASSERT(message);
			}
		}

		Activate(myButtons[myActiveIndex]);
	}


	RadioButtonContainer::~RadioButtonContainer()
	{
		myButtons.DeleteAll();
	}

	void RadioButtonContainer::Update(float aDelta)
	{

	}

	void RadioButtonContainer::Render(const CU::Vector2<float>& aParentPosition)
	{
		myBackground->Render(myPosition + aParentPosition);

		for each(RadioButtonWidget* button in myButtons)
		{
			button->Render(myPosition + aParentPosition);
		}
	}

	Widget* RadioButtonContainer::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			for each (RadioButtonWidget* button in myButtons)
			{
				if (button->IsVisible() == true && button->IsInside(aPosition - myPosition) == true)
				{
					Widget* childWidget = button->MouseIsOver(aPosition - myPosition);
					if (childWidget != nullptr)
					{
						return childWidget;
					}
				}
			}

			return this;
		}
		return nullptr;
	}

	void RadioButtonContainer::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackground->OnResize(aNewSize, anOldSize);

		for each (RadioButtonWidget* button in myButtons)
		{
			button->OnResize(aNewSize, anOldSize);
		}
	}

	void RadioButtonContainer::Activate(RadioButtonWidget* aButton)
	{
		myButtons[myActiveIndex]->Deactivate();
		myActiveIndex = myButtons.Find(aButton);
		DL_ASSERT_EXP(myActiveIndex != myButtons.FoundNone, "Radio button not found.");
		myButtons[myActiveIndex]->Activate();
	}
}