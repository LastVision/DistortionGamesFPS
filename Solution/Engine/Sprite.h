#pragma once

#include "BaseModel.h"
#include <Matrix.h>
#include "ModelLoader.h"

struct ID3D11Texture2D;

namespace Prism
{
	class Sprite : public BaseModel
	{
		friend class Engine;
		friend class ModelLoader;
	public:
		void Render(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale = { 1.f, 1.f }
		, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f }, float aRotateValue = 0.f);

		void SetSize(const CU::Vector2<float>& aTextureSize, const CU::Vector2<float>& aHotSpot);
		const CU::Vector2<float>& GetSize() const;

		void SetUVZeroToOne(const CU::Vector2<float>& aTopLeft, const CU::Vector2<float>& aRightBottom);

		void ResizeTexture(ID3D11Texture2D* aSrcTexture);

		void Rotate(float aRadians);

		void CopyFromD3DTexture(ID3D11Texture2D* aTexture);

		const CU::Vector2<float>& GetHotspot() const;
	private:
		Sprite(const std::string& aFileName, const CU::Vector2<float>& aSpriteSize
			, const CU::Vector2<float>& aHotSpot = { 0.f, 0.f });

		Sprite(ID3D11Texture2D* aTexture, const CU::Vector2<float>& aSpriteSize
			, const CU::Vector2<float>& aHotSpot = { 0.f, 0.f });

		void CreateVertices();


		ID3D11Texture2D* myTexture;
		ID3D11ShaderResourceView* myShaderView;

		CU::Matrix44<float> myOrientation;
		CU::Vector2<float> mySize;
		CU::Vector2<float> myHotspot;
		CU::Vector2<float> myNewSize;
		CU::Vector2<float> myNewHotSpot;

		CU::Vector2<float> myTopLeftUV;
		CU::Vector2<float> myRightBottomUV;
	};
}

inline void Prism::Sprite::SetSize(const CU::Vector2<float>& aTextureSize, const CU::Vector2<float>& aHotSpot)
{
	if (aTextureSize.x != mySize.x || aTextureSize.y != mySize.y
		|| aHotSpot.x != myHotspot.x || aHotSpot.y != myHotspot.y)
	{
		bool shouldPause = !Prism::ModelLoader::GetInstance()->IsPaused();

		if (shouldPause == true)
		{
			ModelLoader::GetInstance()->Pause();
		}
		myHotspot = aHotSpot;
		mySize = aTextureSize;
		CreateVertices();

		if (shouldPause == true)
		{
			ModelLoader::GetInstance()->UnPause();
		}
		//Do the resizing on ModelLoader instead? so we dont have to pause
	}

	myNewSize = aTextureSize;
	myNewHotSpot = aHotSpot;

	//ModelLoader::GetInstance()->Pause();
	//myHotspot = aHotSpot;
	//mySize = aTextureSize;
	//CreateVertices();
	//ModelLoader::GetInstance()->UnPause();
	////Do the resizing on ModelLoader instead? so we dont have to pause
}

inline void Prism::Sprite::SetUVZeroToOne(const CU::Vector2<float>& aTopLeft, const CU::Vector2<float>& aRightBottom)
{
	if (myTopLeftUV == aTopLeft && myRightBottomUV == aRightBottom)
	{
		return;
	}

	bool shouldPause = !Prism::ModelLoader::GetInstance()->IsPaused();

	if (shouldPause == true)
	{
		ModelLoader::GetInstance()->Pause();
	}

	myTopLeftUV = aTopLeft;
	myRightBottomUV = aRightBottom;

	CreateVertices();

	if (shouldPause == true)
	{
		ModelLoader::GetInstance()->UnPause();
	}
}

inline const CU::Vector2<float>& Prism::Sprite::GetSize() const
{
	return mySize;
}

inline const CU::Vector2<float>& Prism::Sprite::GetHotspot() const
{
	return myHotspot;
}