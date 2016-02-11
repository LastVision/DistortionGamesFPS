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
class PlayerDirector;

struct TooltipBox
{
	CU::Vector2<float> myTextPosition;
	CU::Vector2<float> myHeadlinePosition;
	
	Prism::SpriteProxy* myBackground;

	float myHeadlineScale;
	float myTextScale;
};

namespace GUI
{
	class GUIManager;

	class TooltipWidget : public Widget
	{
	public:
		TooltipWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const GUIManager* aGuiManager, const PlayerDirector* aPlayer);
		~TooltipWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;

	private:

		void RenderCost(const CU::Vector2<float>& aParentPosition, const TooltipInfo* aTooltipInfo);

		TooltipBox* myLargeTooltip;
		TooltipBox* mySmallTooltip;

		CU::Vector2<float> myCooldownPosition;
		CU::Vector2<float> myGunpowderPosition;
		CU::Vector2<float> mySupplyPosition;
		CU::Vector2<float> myArtifactPosition;
		CU::Vector2<float> myCostTextOffset;

		Prism::SpriteProxy* myCooldownSprite;
		Prism::SpriteProxy* myGunpowderSprite;
		Prism::SpriteProxy* mySupplySprite;
		Prism::SpriteProxy* myArtifactSprite;

		const Widget* myActiveWidget;
		const GUIManager* myGuiManager;
		const PlayerDirector* myPlayer;
	};
}
