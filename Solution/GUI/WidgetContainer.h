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
	class WidgetContainer : public Widget
	{
	public:

		WidgetContainer(Prism::SpriteProxy* aBackgroundSprite, Prism::SpriteProxy* aVignetteSprite
			, const CU::Vector2<float>& aSize, bool aIsFullscreen, bool aIsScrolling = false);
		~WidgetContainer();

		void AddWidget(Widget* aWidget);
		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		int GetSize() const;

		void SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut) override;

	private:
		CU::GrowingArray<Widget*> myWidgets;
		Prism::SpriteProxy* myBackground;
		Prism::SpriteProxy* myVignette;
		bool myIsScrolling;
		CU::Vector2<float> myScrollOffset;
		CU::Vector2<float> myScrollStaticOffset;
	};

	inline int WidgetContainer::GetSize() const
	{
		return myWidgets.Size();
	}
}