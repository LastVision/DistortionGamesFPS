#pragma once
#include "Widget.h"
#include <Subscriber.h>

namespace Prism
{
	class SpriteProxy;
	class Camera;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

struct MiniMapEvent
{
	Prism::SpriteProxy* myEventSprite;
	float myEventTimer;
	float myEventTime;
	bool myShouldRenderEvent;
	CU::Vector2<float> myEventPosition;
};

namespace GUI
{
	class MiniMapWidget : public Widget, public Subscriber
	{

	public:
		MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const Prism::Camera* aCamera
			, const bool& aCantClickOn, int aLevelID);
		~MiniMapWidget();

		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnRightMouseDown(const CU::Vector2<float>& aPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;


		virtual void ReceiveMessage(const MinimapEventMessage& aMessage) override;

	private:
		void operator=(MiniMapWidget&) = delete;

		void RenderUnits(const CU::Vector2<float>& aParentPosition);
		void RenderVictoryPoints(const CU::Vector2<float>& aParentPosition);
		void RenderResourcePoints(const CU::Vector2<float>& aParentPosition);
		void RenderBases(const CU::Vector2<float>& aParentPosition);
		void RenderArtifacts(const CU::Vector2<float>& aParentPosition);

		Prism::SpriteProxy* myPlaceholderSprite;

		Prism::SpriteProxy* myUnitSprite;
		Prism::SpriteProxy* myBaseSprite;
		Prism::SpriteProxy* myResourcePointSprite;
		Prism::SpriteProxy* myVictoryPointSprite;
		Prism::SpriteProxy* myArtifactSprite;
		Prism::SpriteProxy* myCameraFrustum;

		Prism::SpriteProxy* myEventSprite;
		CU::GrowingArray<MiniMapEvent> myEventSprites;
		int myCurrentEventSprite;

		const CU::Matrix44<float>* myCameraOrientation;
		const bool& myCantClickOn;

	};
}