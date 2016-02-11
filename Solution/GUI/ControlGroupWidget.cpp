#include "stdafx.h"
#include "ButtonWidget.h"
#include "ControlGroupWidget.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"

namespace GUI
{
	ControlGroupWidget::ControlGroupWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer
		, CU::Vector2<float> aSize)
		: Widget()
		, myControlGroups(aPlayer->GetControlGroups())
		, myControlGroupButtons(AMOUNT_OF_CONTROL_GROUPS)
	{
		myPosition = { 0.f, 0.f };
		mySize = aSize;
	
		tinyxml2::XMLElement* widgetElement = aReader->FindFirstChild(anXMLElement, "widget");
		for (; widgetElement != nullptr; widgetElement = aReader->FindNextElement(widgetElement))
		{
			std::string type = "";
			aReader->ForceReadAttribute(widgetElement, "type", type);

			if (type == "button")
			{
				ButtonWidget* button = new ButtonWidget(aReader, widgetElement, aPlayer);
				myControlGroupButtons.Add(button);
			}
			else
			{
				DL_ASSERT("[GUI] wrong widget type in ControlGroup widget");
			}
		}
	}

	ControlGroupWidget::~ControlGroupWidget()
	{
		myControlGroupButtons.DeleteAll();
	}

	void ControlGroupWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		CU::Vector2<float> position = myPosition + aParentPosition;

		for (int i = 0; i < AMOUNT_OF_CONTROL_GROUPS; i++)
		{
			if (myControlGroups[i].Size() > 0)
			{
				myControlGroupButtons[i]->Render(myPosition + aParentPosition);
			}
		}
	}

	void ControlGroupWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		for (int i = 0; i < myControlGroupButtons.Size(); i++)
		{
			myControlGroupButtons[i]->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		}
	}

	Widget* ControlGroupWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		for (int i = 0; i < myControlGroupButtons.Size(); i++)
		{
			if (myControlGroups[i].Size() > 0 && myControlGroupButtons[i]->IsInside(aPosition - myPosition) == true)
			{
				return myControlGroupButtons[i];
			}
		}
		return nullptr;
	}
}