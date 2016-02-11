#pragma once

#include "BaseModel.h"

namespace Prism
{
	class DebugFont;

	class DebugText : public BaseModel
	{
	public:
		DebugText();
		void Init(DebugFont* aFont);

		void Render(const std::string& aString, const CU::Vector2<float>& aPosition
			, const CU::Vector2<float>& aScale = {1.f, 1.f}
			, const CU::Vector4<float>& aColor = CU::Vector4<float>(1.f, 1.f, 1.f, 1.f));


		float GetTextWidth() const;
		CU::Vector2<float> GetTextSize(const std::string& aString) const;
		const CU::Vector2<float>& GetCharSize() const;

	private:
		void ConstructBuffers(const std::string& aString);

		CU::GrowingArray<VertexPosUV> myVertices;
		CU::GrowingArray<int> myIndices;

		DebugFont* myFont;

		CU::Vector2<float> myCharSize;
		float myTextWidth;
		float myCharSpacing;

		std::string myLastText;
	};
}