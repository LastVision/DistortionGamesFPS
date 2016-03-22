#include "GameWrapper.h"

#include <Engine.h>
#include <DL_Debug.h>

GameWrapper::GameWrapper()
{
	DL_Debug::Debug::Create();
	Prism::Engine::CreateOcculus();
}


GameWrapper::~GameWrapper()
{
	Prism::Engine::Destroy();
	DL_Debug::Debug::Destroy();
}

void GameWrapper::SetContext(ID3D11DeviceContext* aContext)
{
	Prism::Engine::GetInstance()->SetContext(aContext);
}

void GameWrapper::SetDevice(ID3D11Device* aDevice)
{
	Prism::Engine::GetInstance()->SetDevice(aDevice);
}

void GameWrapper::Init()
{
	myModel = new Prism::Model();
	myModel->InitCube();
	myOrientation.SetPos(CU::Vector3<float>(0, 0, 1.5f));
}

void GameWrapper::Render(const DirectX::XMMATRIX& aViewProjection)
{
	static bool firstFrame = true;
	if (firstFrame == true)
	{
		firstFrame = false;
	}
	else
	{
	myModel->RenderOcculus(myOrientation, ConvertMatrix(aViewProjection));

	}
}

CU::Matrix44<float> GameWrapper::ConvertMatrix(const DirectX::XMMATRIX& aMatrix)
{
	CU::Matrix44<float> toReturn;

	toReturn.myMatrix[0] = aMatrix.r[0].m128_f32[0];
	toReturn.myMatrix[1] = aMatrix.r[0].m128_f32[1];
	toReturn.myMatrix[2] = aMatrix.r[0].m128_f32[2];
	toReturn.myMatrix[3] = aMatrix.r[0].m128_f32[3];
	toReturn.myMatrix[4] = aMatrix.r[1].m128_f32[0];
	toReturn.myMatrix[5] = aMatrix.r[1].m128_f32[1];
	toReturn.myMatrix[6] = aMatrix.r[1].m128_f32[2];
	toReturn.myMatrix[7] = aMatrix.r[1].m128_f32[3];
	toReturn.myMatrix[8] = aMatrix.r[2].m128_f32[0];
	toReturn.myMatrix[9] = aMatrix.r[2].m128_f32[1];
	toReturn.myMatrix[10] = aMatrix.r[2].m128_f32[2];
	toReturn.myMatrix[11] = aMatrix.r[2].m128_f32[3];
	toReturn.myMatrix[12] = aMatrix.r[3].m128_f32[0];
	toReturn.myMatrix[13] = aMatrix.r[3].m128_f32[1];
	toReturn.myMatrix[14] = aMatrix.r[3].m128_f32[2];
	toReturn.myMatrix[15] = aMatrix.r[3].m128_f32[3];

	return toReturn;
}