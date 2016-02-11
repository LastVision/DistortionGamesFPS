#pragma once
#include "Widget.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class SpriteProxy;
}

class XMLReader;
class Entity;
class BuildingComponent;
class PlayerDirector;
class AIDirector;

namespace GUI
{
	class BarWidget;

	class UnitInfoWidget : public Widget
	{

	public:
		UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer
			, const AIDirector* anAI);
		~UnitInfoWidget();

		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnMousePressed(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;

		Widget*	MouseIsOver(const CU::Vector2<float>& aPosition) override;

	private:
		void RenderBaseInfo(const CU::Vector2<float>& aParentPosition);
		void RenderUnitInfo(const CU::Vector2<float>& aParentPosition);
		void RenderUnitGroupInfo(const CU::Vector2<float>& aParentPosition);

		void operator=(UnitInfoWidget&) = delete;

		const CU::GrowingArray<Entity*>& myUnits;
		const BuildingComponent& myPlayerBuilding;
		const BuildingComponent& myEnemyBuilding;

		CU::GrowingArray<ButtonWidget*> myQueueButtons;

		Prism::SpriteProxy* myGruntUnit;
		Prism::SpriteProxy* myRangerUnit;
		Prism::SpriteProxy* myTankUnit;
		Prism::SpriteProxy* myGruntPortrait;
		Prism::SpriteProxy* myRangerPortrait;
		Prism::SpriteProxy* myTankPortrait;
		Prism::SpriteProxy* myBuildingPortrait;
		Prism::SpriteProxy* myStatsSprite;
		Prism::SpriteProxy* myActiveQueueOverlay;
		Prism::SpriteProxy* myUpgradeStar;
		BarWidget* myBuildingTimer;

		eEntityType mySelectedType;
		bool myIsUnitSelected;

		CU::Vector4<float> myEnemyColor;
		CU::Vector2<float> myUnitPosition;
		CU::Vector2<float> myPortraitPosition;
		CU::Vector2<float> myTimerPosition;
		CU::Vector2<float> myUpgradeStarPosition;
		float myTextScale;
	};
}