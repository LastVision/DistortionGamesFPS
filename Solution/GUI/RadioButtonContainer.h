#pragma once
#include "Widget.h"
#include <GrowingArray.h>

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class RadioButtonWidget;
	class SpriteWidget;

	class RadioButtonContainer : public Widget
	{
	public:
		RadioButtonContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~RadioButtonContainer();

		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		void Activate(RadioButtonWidget* aButton);

	private:
		CU::GrowingArray<RadioButtonWidget*> myButtons;
		SpriteWidget* myBackground;
		int myActiveIndex;
	};
}