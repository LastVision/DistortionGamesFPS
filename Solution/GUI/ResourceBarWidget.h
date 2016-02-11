#pragma once
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
	class SpriteAnimator;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
class PlayerDirector;
class AIDirector;

namespace GUI
{

	class ResourceBarWidget : public Widget
	{

	public:
		ResourceBarWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer, const AIDirector* anAI);
		~ResourceBarWidget();
		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;

	private:
		void operator=(ResourceBarWidget&) = delete;

		const int* myValue;
		int myLastValue;

		Prism::SpriteProxy* myValueSprite;

		CU::Vector2<float> mySpritePosition;
		CU::Vector2<float> myTextPosition;

		Prism::SpriteAnimator* myGainAnimation;

		float myTextScale;
	};
}