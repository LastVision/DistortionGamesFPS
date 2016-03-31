#pragma once
#include <Vector.h>

namespace Prism
{
	class Camera;
	class Text;
	class TextProxy
	{
	public:
		TextProxy();
		~TextProxy();

		void SetPosition(const CU::Vector2<float>& aPosition);
		void SetOffset(const CU::Vector3<float>& aOffset);
		void Rotate3dText(float aRadians);
		void SetText(const std::string& aText);
		void SetColor(const CU::Vector4<float>& aColor);
		void Render();
		void Render(const Camera* aCamera);
		void SetScale(const CU::Vector2<float>& aScale);
		float GetWidth() const;

		bool IsLoaded() const;
		void SetText(Text* aText);

	private:
		Text* myText;
			
		CU::Vector2<float> myPosition;
		CU::Vector2<float> myScale;
		CU::Vector4<float> myColor;


		CU::Matrix44<float> my3DOrientation;
	};
}