#pragma once
#include "Widget.h"
#include <OnRadioButtonMessage.h>

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
struct OnClickMessage;

namespace GUI
{
	class RadioButtonContainer;

	class RadioButtonWidget : public Widget
	{
	public:
		RadioButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, eOnRadioButtonEvent aRadioButtonType, RadioButtonContainer& aContainer);
		~RadioButtonWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnLeftMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnLeftMouseUp() override;
		void OnMouseEnter() override;
		void OnMouseExit() override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		bool IsInside(const CU::Vector2<float>& aPosition) const override;
		void SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot = true) override;

		void SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut) override;

		void Deactivate();
		void Activate();

	private:
		void operator=(RadioButtonWidget&) = delete;
		void Click();

		int myId;
		bool myActive;

		RadioButtonContainer& myContainer;

		std::string myHoverText;
		std::string myButtonText;
		bool myIsTextButton;

		Prism::SpriteProxy* myImageNormal;
		Prism::SpriteProxy* myImagePressed;
		Prism::SpriteProxy* myImageHover;
		Prism::SpriteProxy* myImageActive;

		Prism::SpriteProxy* myImageCurrent;

		eOnRadioButtonEvent myType;
	};
}