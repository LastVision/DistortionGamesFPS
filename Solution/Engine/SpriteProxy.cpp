#include "stdafx.h"
#include "SpriteProxy.h"

namespace Prism
{
	SpriteProxy::SpriteProxy()
		: mySprite(nullptr)
		, myTopLeftUV(0.f, 0.f)
		, myRightBottomUV(1.f, 1.f)
	{
	}

	void SpriteProxy::Render(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale, const CU::Vector4<float>& aColor)
	{
		if (mySprite != nullptr)
		{
			mySprite->SetSize(mySize, myHotspot);
			mySprite->SetUVZeroToOne(myTopLeftUV, myRightBottomUV);
			mySprite->Render(aPosition, aScale, aColor);
		}
	}

	void SpriteProxy::SetSize(const CU::Vector2<float>& aTextureSize, const CU::Vector2<float>& aHotSpot)
	{
		mySize = aTextureSize;
		myHotspot = aHotSpot;
	}

	const CU::Vector2<float>& SpriteProxy::GetSize() const
	{
		/*if (mySprite != nullptr)
		{
			return mySprite->GetSize();
		}*/

		return mySize;
	}

	void SpriteProxy::SetUVZeroToOne(const CU::Vector2<float>& aTopLeft, const CU::Vector2<float>& aRightBottom)
	{
		if (aTopLeft.x < 0.f || aTopLeft.x > 1.f || aTopLeft.y < 0.f || aTopLeft.y > 1.f ||
			aRightBottom.x < 0.f || aRightBottom.x > 1.f || aRightBottom.y < 0.f || aRightBottom.y > 1.f)
		{
			DL_ASSERT("[Sprite] UV coordinates are set wrong. Has to be in a zero to one space.");
		}

		myTopLeftUV = aTopLeft;
		myRightBottomUV = aRightBottom;
	}

	void SpriteProxy::ResizeTexture(ID3D11Texture2D* aSrcTexture)
	{
		if (mySprite != nullptr)
		{
			mySprite->ResizeTexture(aSrcTexture);
		}
	}

	void SpriteProxy::Rotate(float aRadians)
	{
		if (mySprite != nullptr)
		{
			mySprite->Rotate(aRadians);
		}
	}

	void SpriteProxy::CopyFromD3DTexture(ID3D11Texture2D* aTexture)
	{
		if (mySprite != nullptr)
		{
			mySprite->CopyFromD3DTexture(aTexture);
		}
	}

	const CU::Vector2<float>& SpriteProxy::GetHotspot() const
	{
		/*if (mySprite != nullptr)
		{
			return mySprite->GetHotspot();
		}*/

		return myHotspot;
	}
}