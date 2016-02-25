#pragma once
#include "Widget.h"

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
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~ButtonWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnLeftMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnLeftMouseUp() override;
		void OnMouseEnter() override;
		void OnMouseExit() override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		bool IsInside(const CU::Vector2<float>& aPosition) const override;
		void SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot = true) override;

		void ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		
		void SetEvent(OnClickMessage* anEvent);
		const OnClickMessage* GetEvent() const;

		void SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut) override;

	protected:
		void Click();

		int myId;

		std::string myHoverText;
		std::string myButtonText;
		bool myIsTextButton;
		
		Prism::SpriteProxy* myImageNormal;
		Prism::SpriteProxy* myImagePressed;
		Prism::SpriteProxy* myImageHover;
		Prism::SpriteProxy* myImageCurrent;

		OnClickMessage* myClickEvent;
	};

	inline const OnClickMessage* ButtonWidget::GetEvent() const
	{
		return myClickEvent;
	}

	inline void ButtonWidget::SetEvent(OnClickMessage* anEvent)
	{
		myClickEvent = anEvent;
	}
}