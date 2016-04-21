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

#include <XMLReader.h>
#include <MathHelper.h>
#include <PointLight.h>

#include <ClientGame.h>
#include <ClientLevel.h>
#include <ClientLevelFactory.h>

#include <Cursor.h>
#include <PostMaster.h>
#include <AudioInterface.h>
#include <ClientNetworkManager.h>
#include <GameConstants.h>

GameWrapper::GameWrapper(float aHeight, float aWidth, ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
{
	DL_Debug::Debug::Create();
	Prism::Engine::CreateOcculus(aHeight, aWidth, aDevice, aContext);
}


GameWrapper::~GameWrapper()
{
	Prism::Audio::AudioInterface::Destroy();
	PostMaster::Destroy();
	ClientNetworkManager::Destroy();
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
	//CU::InputWrapper::Create(GetActiveWindow(), GetModuleHandle(NULL), DISCL_NONEXCLUSIVE
	//	| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	//Prism::Audio::AudioInterface::CreateInstance();
	//PostMaster::Create();
	//ClientNetworkManager::Create();

	myGame = new ClientGame();
	myGame->Init(GetActiveWindow());

	//myCamera = new Prism::Camera(myPlayerMatrix);


	//myCursor = new GUI::Cursor(Prism::Engine::GetInstance()->GetWindowSizeInt());;

	//myLevelFactory = new ClientLevelFactory("Data/Level/LI_level.xml");

	//eStateStatus status = eStateStatus::eKeepState;
	//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(0, myCursor, status));
	//myLevel->SetCamera(myCamera);
}

void GameWrapper::Update(float aDelta)
{
	myGame->Update();
}

void GameWrapper::Render(const DirectX::XMMATRIX& aViewProjection, ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthStencil)
{
	//myLevel->Render(aRenderTarget, aDepthStencil);

	Prism::Engine::GetInstance()->SetBackBuffer(aRenderTarget);
	Prism::Engine::GetInstance()->SetDepthStencil(aDepthStencil);

	myGame->Render();
}

void GameWrapper::SetMatrices(const CU::Matrix44<float>& aView, const CU::Matrix44<float>& aProjection, const CU::Matrix44<float>& aViewProjection)
{
	GC::View = aViewProjection;
	GC::Projection = aProjection;
	GC::ViewProjection = aViewProjection;
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