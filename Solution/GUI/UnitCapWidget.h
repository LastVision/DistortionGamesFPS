#pragma once
#include "Widget.h"

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class UnitCapWidget : public Widget
	{

	public:
		UnitCapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement
			, const CU::Vector2<float>& aSize, const int& aUnitCap, const int& aUnitCount);
		~UnitCapWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;

	private:
		void operator=(UnitCapWidget&) = delete;

		const int& myUnitCap;
		const int& myUnitCount;

		float myTextScale;
	};
}