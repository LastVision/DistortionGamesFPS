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
		void Render(const CU::Vector2<float>& aParentPosition, int anIndex);

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;
		Widget* MouseIsOver(const CU::Vector2<float>& aPosition, int anIndex);

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen) override;

		int GetSize() const;

		Widget* GetFirstWidget();

		Widget* At(const int& aIndex);
		const Widget* At(const int& aIndex) const;

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

	inline Widget* WidgetContainer::GetFirstWidget()
	{
		DL_ASSERT_EXP(myWidgets.Size() > 0, "Trying to get first widget from an empty container");
		return myWidgets[0];
	}
	
	inline Widget* WidgetContainer::At(const int& aIndex)
	{
		return myWidgets[aIndex];
	}

	inline const Widget* WidgetContainer::At(const int& aIndex) const
	{
		return myWidgets[aIndex];
	}
}