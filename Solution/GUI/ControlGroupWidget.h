#pragma once
#include "Widget.h"
#include <StaticArray.h>
#include "../Game/GameDefines.h"

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
class Entity;
class PlayerDirector;

namespace GUI
{
	class ButtonWidget;

	class ControlGroupWidget : public Widget
	{

	public:
		ControlGroupWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer
			, CU::Vector2<float> aSize);
		~ControlGroupWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;
		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

	private:
		void operator=(ControlGroupWidget&) = delete;

		const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& myControlGroups;

		CU::GrowingArray<ButtonWidget*> myControlGroupButtons;
	};
}