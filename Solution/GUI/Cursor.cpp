#include "stdafx.h"
#include <CommonHelper.h>
#include "Cursor.h"
#include "../InputWrapper/InputWrapper.h"

namespace GUI
{
	Cursor::Cursor(const CU::Vector2<int>& aWindowSize)
		: myCurrentType(eCursorType::NORMAL)
		, mySprites(8)
		, myShouldRender(true)
	{
		myWindowSize.x = float(aWindowSize.x);
		myWindowSize.y = float(aWindowSize.y);

		Prism::SpriteProxy* normalSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Crosshair/T_crosshair_default.dds", { 70.f, 70.f }, { 35.f, 35.f });
		mySprites.Add(normalSprite);

		Prism::SpriteProxy* attackSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Crosshair/T_crosshair_attack.dds", { 70.f, 70.f }, { 35.f, 35.f });
		mySprites.Add(attackSprite);

		Prism::SpriteProxy* attackMoveSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Crosshair/T_crosshair_attack_move.dds", { 70.f, 70.f }, { 35.f, 35.f });
		mySprites.Add(attackMoveSprite);

		Prism::SpriteProxy* totemSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Crosshair/T_crosshair_totem.dds", { 70.f, 70.f }, { 35.f, 35.f });
		mySprites.Add(totemSprite);

		Prism::SpriteProxy* cancelSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Crosshair/T_crosshair_cancel.dds", { 70.f, 70.f }, { 35.f, 35.f });
		mySprites.Add(cancelSprite);

		myPosition = myWindowSize / 2.f;

		myPositionZeroToOne = myPosition / myWindowSize;

	}

	Cursor::~Cursor()
	{
		mySprites.DeleteAll();
	}

	void Cursor::Update()
	{
		// uncomment for software cursor:
		//myPosition.x += myInputWrapper->GetMouseDX();
		//myPosition.y -= myInputWrapper->GetMouseDY();

		myPosition.x = CU::InputWrapper::GetInstance()->GetMousePosition().x;
		myPosition.y = myWindowSize.y - CU::InputWrapper::GetInstance()->GetMousePosition().y;

		myPositionZeroToOne = myPosition / myWindowSize;
		myPositionZeroToOne.x = CU::Clip(myPositionZeroToOne.x, 0, 1.f);
		myPositionZeroToOne.y = CU::Clip(myPositionZeroToOne.y, 0, 1.f);
	}

	void Cursor::Render()
	{
		if (myShouldRender == true)
		{
			mySprites[static_cast<int>(myCurrentType)]->Render(myPosition);
		}
	}

	const CU::Vector2<float>& Cursor::GetMousePosition() const
	{
		return myPosition;
	}

	const CU::Vector2<float>& Cursor::GetMousePositionZeroToOne() const
	{
		return myPositionZeroToOne;
	}

	void Cursor::OnResize(int aWidth, int aHeight)
	{
		myWindowSize.x = float(aWidth);
		myWindowSize.y = float(aHeight);
	}

	void Cursor::ClipCursor()
	{
		if (myPosition.x < 0.f)
		{
			myPosition.x = 0.f;
		}
		else if (myPosition.x > myWindowSize.x)
		{
			myPosition.x = myWindowSize.x;
		}

		if (myPosition.y < 0.f)
		{
			myPosition.y = 0.f;
		}
		else if (myPosition.y > myWindowSize.y)
		{
			myPosition.y = myWindowSize.y;
		}
	}
}