#pragma once

#define MAXNROFCUBES 64
namespace Prism
{

	class Cube3D;
	class Cube3DRenderer
	{
	public:
		Cube3DRenderer();
		~Cube3DRenderer();

		void AddCube(const CU::Vector3<float>& aPosition, float aSideLength, const CU::Vector4<float>& aColor
			, bool aWireFrame);

		void Render(const Camera& aCamera);

	private:
		CU::GrowingArray<Cube3D*> myInactiveCubes;
		CU::GrowingArray<Cube3D*> myActiveCubes;
	};
}

