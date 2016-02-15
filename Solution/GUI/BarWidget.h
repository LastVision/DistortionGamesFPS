#pragma once
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
}

namespace GUI
{
	class BarWidget : public Widget
	{
	public:
		BarWidget(const int& aMaxValue, const int& aCurrentValue, CU::Vector2<float> aSize, CU::Vector4<float> aColor);
		BarWidget(const float& aMaxValue, const float& aCurrentValue, CU::Vector2<float> aSize, CU::Vector4<float> aColor);
		~BarWidget();

		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void operator=(BarWidget&) = delete;

		Prism::SpriteProxy* myBackgroundSprite;
		Prism::SpriteProxy* myValueSprite;

		bool myIsFloat;

		CU::Vector4<float> myColor;
		
		union
		{
			const float* myCurrentValueFloat;
			const int* myCurrentValueInt;
		};

		union
		{
			const float* myMaxValueFloat;
			const int* myMaxValueInt;
		};
	};			   
}