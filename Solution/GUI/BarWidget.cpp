#include "stdafx.h"
#include "BarWidget.h"

namespace GUI
{
	BarWidget::BarWidget(const int& aMaxValue, const int& aCurrentValue, CU::Vector2<float> aSize, CU::Vector4<float> aColor)
		: Widget()
		, myMaxValueInt(&aMaxValue)
		, myCurrentValueInt(&aCurrentValue)
		, myIsFloat(false)
		, myColor(aColor)
	{
		mySize = aSize;

		CU::Vector2<float> backgroundSize = mySize + 8.f;

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);

		myValueSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);
	}

	BarWidget::BarWidget(const float& aMaxValue, const float& aCurrentValue, CU::Vector2<float> aSize, CU::Vector4<float> aColor)
		: myMaxValueFloat(&aMaxValue)
		, myCurrentValueFloat(&aCurrentValue)
		, myIsFloat(true)
		, myColor(aColor)
	{
		mySize = aSize;

		CU::Vector2<float> backgroundSize = mySize + 8.f;

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);

		myValueSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);
	}

	BarWidget::~BarWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
		SAFE_DELETE(myValueSprite);
	}

	void BarWidget::Update(float)
	{
		if (myIsFloat == false)
		{
			if (*myCurrentValueInt * mySize.x != myValueSprite->GetSize().x)
			{
				float newSize = float(*myCurrentValueInt) / float(*myMaxValueInt);
				myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
			}
		}
		else
		{
			if (*myCurrentValueFloat * mySize.x != myValueSprite->GetSize().x)
			{
				float newSize = *myCurrentValueFloat / *myMaxValueFloat;
				myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
			}
		}
	}

	void BarWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myBackgroundSprite->Render(myPosition + aParentPosition);
		myValueSprite->Render(myPosition + aParentPosition, { 1.f, 1.f }, myColor);
	}

	void BarWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		float newSize;

		if (myIsFloat == false)
		{
			newSize = float(*myCurrentValueInt) / float(*myMaxValueInt);
		}
		else
		{
			newSize = *myCurrentValueFloat / *myMaxValueFloat;
		}

		//CU::Vector2<float> ratio = myBackgroundSprite->GetSize() / anOldWindowSize.x;
		myBackgroundSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
		myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
	}
}