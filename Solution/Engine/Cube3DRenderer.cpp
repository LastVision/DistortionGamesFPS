#include "stdafx.h"
#include "Cube3D.h"
#include "Cube3DRenderer.h"
#include "DebugDrawer.h"

namespace Prism
{
	Cube3DRenderer::Cube3DRenderer()
		: myInactiveCubes(MAXNROFCUBES)
		, myActiveCubes(MAXNROFCUBES)
	{
		for (int i = 0; i < MAXNROFCUBES; ++i)
		{
			myInactiveCubes.Add(new Cube3D());
		}
	}


	Cube3DRenderer::~Cube3DRenderer()
	{
		myInactiveCubes.DeleteAll();
		myActiveCubes.DeleteAll();
	}

	void Cube3DRenderer::AddCube(const CU::Vector3<float>& aPosition, float aSideLength, const CU::Vector4<float>& aColor
		, bool aWireFrame)
	{
#ifndef RELEASE_BUILD
		ModelLoader::GetInstance()->Pause();
		myActiveCubes.Add(myInactiveCubes.GetLast());
		myInactiveCubes.RemoveCyclicAtIndex(myInactiveCubes.Size() - 1);
		myActiveCubes.GetLast()->SetPosition(aPosition);
		myActiveCubes.GetLast()->SetSizeAndColor(aSideLength, aColor);
		myActiveCubes.GetLast()->SetWireFrame(aWireFrame);
		ModelLoader::GetInstance()->UnPause();
#endif
	}

	void Cube3DRenderer::Render(const Camera& aCamera)
	{
		for (int i = myActiveCubes.Size() - 1; i >= 0; --i)
		{
			myActiveCubes[i]->Render(aCamera);
			myInactiveCubes.Add(myActiveCubes[i]);
			myActiveCubes.RemoveCyclicAtIndex(i);
		}
	}
}