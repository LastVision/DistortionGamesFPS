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

namespace GUI
{

	class SpriteWidget : public Widget
	{
	public:
		SpriteWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~SpriteWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

	private:
		Prism::SpriteProxy* myBackground;
	};
}
