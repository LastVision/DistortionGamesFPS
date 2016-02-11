#pragma once
struct ID3D11Texture2D;
namespace Prism
{
	class Sprite;

	class SpriteProxy
	{
	friend class ModelLoader;

	public:
		SpriteProxy();

		void Render(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale = { 1.f, 1.f }
			, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });

		void SetSize(const CU::Vector2<float>& aTextureSize, const CU::Vector2<float>& aHotSpot);
		const CU::Vector2<float>& GetSize() const;

		void SetUVZeroToOne(const CU::Vector2<float>& aTopLeft, const CU::Vector2<float>& aRightBottom);

		void ResizeTexture(ID3D11Texture2D* aSrcTexture);

		void Rotate(float aRadians);

		void CopyFromD3DTexture(ID3D11Texture2D* aTexture);

		const CU::Vector2<float>& GetHotspot() const;

	private:

		Sprite* mySprite;
		CU::Vector2<float> mySize;
		CU::Vector2<float> myHotspot;

		CU::Vector2<float> myTopLeftUV;
		CU::Vector2<float> myRightBottomUV;
	};
}