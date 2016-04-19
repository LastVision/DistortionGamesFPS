#pragma once
#include <string>
#include "BaseModel.h"
#include "Vertices.h"

namespace Prism
{
	class Camera;
	class FontProxy;

	class Text : public BaseModel
	{
	public:
		Text(const FontProxy& aFont, bool aIs3d = false, bool aShouldFollowCamera = false);
		void SetPosition(const CU::Vector2<float>& aPosition);
		void SetText(const std::string& aText);
		void SetColor(const CU::Vector4<float>& aColor);
		void Render(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale, const CU::Vector4<float>& aColor);
		void Render(const Camera* aCamera, const CU::Matrix44<float>& aOrientation, const CU::Vector4<float>& aColor);
		void Render(const Camera* aCamera, const CU::Matrix44<float>& aOrientation, const CU::Vector2<float>& aScale, const CU::Vector4<float>& aColor);
		void SetScale(const CU::Vector2<float>& aScale);
		float GetWidth() const;

		const std::string& GetText() const;

	private:
		void operator=(Text&) = delete;
		void ConstructBuffers();
		const FontProxy& myFont;

		CU::GrowingArray<VertexPosUV> myVertices;
		CU::GrowingArray<int> myIndices;

		std::string myText;

		CU::Vector4<float> myColor;

		bool myShouldFollowCamera;

	};
}

inline void Prism::Text::SetPosition(const CU::Vector2<float>& aPosition)
{
	myPosition.x = static_cast<float>(static_cast<int>(aPosition.x + 0.5f));
	myPosition.y = static_cast<float>(static_cast<int>(aPosition.y + 0.5f));
	myPosition.y -= Engine::GetInstance()->GetWindowSize().y;
}

inline void Prism::Text::SetScale(const CU::Vector2<float>&)
{
	myScale = myScale; // scaling not allowed
}

inline void Prism::Text::SetColor(const CU::Vector4<float>& aColor)
{
	myColor = aColor;
}

inline const std::string& Prism::Text::GetText() const
{
	return myText;
}