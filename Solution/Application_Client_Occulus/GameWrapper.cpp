#include "GameWrapper.h"
#include "GameEnum.h"

#include <Engine.h>
#include <DL_Debug.h>


#include <ModelLoader.h>
#include <ModelProxy.h>

#include <Scene.h>
#include <DeferredRenderer.h>
#include <Instance.h>
#include <InputWrapper.h>
#include <Camera.h>

GameWrapper::GameWrapper(float aHeight, float aWidth, ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
{
	DL_Debug::Debug::Create();
	Prism::Engine::CreateOcculus(aHeight, aWidth, aDevice, aContext);
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

void GameWrapper::SetWindowSize(const CU::Vector2<float>& aWindowSize)
{
	Prism::Engine::GetInstance()->SetWindowSize(aWindowSize);
}

void GameWrapper::Init()
{
	CU::InputWrapper::Create(GetActiveWindow(), GetModuleHandle(NULL), DISCL_NONEXCLUSIVE
		| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	myScene = new Prism::Scene();
	myRenderer = new Prism::DeferredRenderer();
	myCamera = new Prism::Camera(myPlayerMatrix);
	myScene->SetCamera(*myCamera);

	myModels.Init(8);
	Prism::Model* model = new Prism::Model();
	model->InitCube(1.f, 1.f, 1.f, CU::Vector4<float>(1.f, 1.f, 1.f, 1.f));
	myModels.Add(model);

	Prism::Model* model1 = new Prism::Model();
	model1->InitCube(1.f, 1.f, 1.f, CU::Vector4<float>(0.f, 1.f, 0.f, 1.f));
	myModels.Add(model1);

	Prism::Model* model2 = new Prism::Model();
	model2->InitCube(1.f, 1.f, 1.f, CU::Vector4<float>(0.f, 0.f, 1.f, 1.f));
	myModels.Add(model2);

	myOrientations.Init(8);
	myOrientations.Add(CU::Matrix44<float>());
	myOrientations.Add(CU::Matrix44<float>());
	myOrientations.Add(CU::Matrix44<float>());
	myOrientations.Add(CU::Matrix44<float>());

	myOrientations[0].SetPos(CU::Vector3<float>(1.5f, 0, 0));
	myOrientations[1].SetPos(CU::Vector3<float>(0, 1.5f, 0));
	myOrientations[2].SetPos(CU::Vector3<float>(0, 0, 1.5f));
	myOrientations[3].SetPos(CU::Vector3<float>(0, 0, -1.5f));

	Prism::ModelProxy* test = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Pickups/Healthpack/SM_health_pack.fbx", "Data/Resource/Shader/S_effect_pbl_deferred.fx");

	Prism::ModelProxy* test2 = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/Enemy/SK_cyborg_3_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myAnimation = new Prism::Instance(*test2, myOrientations[3]);

	while (test->IsLoaded() == false);

	myScene->AddInstance(new Prism::Instance(*test, myOrientations[0]), eObjectRoomType::ALWAYS_RENDER);
	myScene->AddInstance(new Prism::Instance(*test, myOrientations[1]), eObjectRoomType::ALWAYS_RENDER);
	myScene->AddInstance(new Prism::Instance(*test, myOrientations[2]), eObjectRoomType::ALWAYS_RENDER);
	myScene->AddInstance(myAnimation, eObjectRoomType::ALWAYS_RENDER);
}

void GameWrapper::Update(float aDelta, const CU::Matrix44<float>& aView, const CU::Matrix44<float>& aProjection, const CU::Matrix44<float>& aViewProjection)
{
	myCamera->SetOrientation(aView);
	myCamera->SetProjection(aProjection);
	myCamera->SetViewProjection(aViewProjection);
	myCamera->Update(aDelta);

	myOrientations[0] *= CU::Matrix44<float>::CreateRotateAroundX(aDelta);

	myAnimation->Update(aDelta);
}

void GameWrapper::Render(const DirectX::XMMATRIX& aViewProjection, ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthStencil)
{
	for (int i = 0; i < myModels.Size(); ++i)
	{
	//	myModels[i]->RenderOcculus(myOrientations[i], ConvertMatrix(aViewProjection));
	}

	myRenderer->Render(myScene, aRenderTarget, aDepthStencil);
}

CU::Matrix44<float> GameWrapper::ConvertMatrix(const DirectX::XMMATRIX& aMatrix)
{
	CU::Matrix44<float> toReturn;

	toReturn.myMatrix[0] = -aMatrix.r[0].m128_f32[0];
	toReturn.myMatrix[1] = -aMatrix.r[0].m128_f32[1];
	toReturn.myMatrix[2] = -aMatrix.r[0].m128_f32[2];
	toReturn.myMatrix[3] = -aMatrix.r[0].m128_f32[3];
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