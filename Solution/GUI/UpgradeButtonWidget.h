#pragma once
#include "../Game/GameDefines.h"
#include "Widget.h"
#include <StaticArray.h>

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
class PlayerDirector;

namespace GUI
{
	class AbilityButton;

	class UpgradeButtonWidget : public Widget
	{
	public:
		UpgradeButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer);
		~UpgradeButtonWidget();

		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen) override;

	private:
		void operator=(UpgradeButtonWidget&) = delete;

		CU::StaticArray<AbilityButton*, AMOUNT_OF_UPGRADES> myUpgrades;

		const int* myUpgradeLevel;
	};
}