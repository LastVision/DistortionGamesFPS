#include <string>
#include <Camera.h>
#include <DirectionalLight.h>
#include <DebugDrawer.h>
#include <DL_Debug.h>
#include "DLLApp.h"
#include "DLLCamera.h"
#include "DLLModel.h"
#include "DLLParticle.h"
#include <EffectContainer.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <FileWatcher.h>
#include <Instance.h>
#include <InputWrapper.h>
#include <Matrix.h>
#include <Scene.h>
#include <SetupInfo.h>
#include <TimerManager.h>

#define EngineInstance Prism::Engine::GetInstance()
#define InputInstance CU::InputWrapper::GetInstance()

DLLApp::DLLApp(int* aHwnd, Prism::SetupInfo& aWindowSetup, WNDPROC aWindowProc)
{
	DL_Debug::Debug::Create();
	CU::TimerManager::Create();

	myPanelWindowHandler = (HWND)aHwnd;

	Prism::Engine::Create(myEngineWindowHandler, aWindowProc, aWindowSetup);

	SetupLight();
	SetupInput();
	SetParentWindow(aWindowSetup);
	SetupCubeMap();

	CU::Vector2<float> windowSize(aWindowSetup.myScreenWidth, aWindowSetup.myScreenHeight);
	myCamera = new DLLCamera(windowSize, 1.0f, 1.0f, 1.0f);
	myModel = new DLLModel();
	myParticle = new DLLParticle();

	
}

DLLApp::~DLLApp()
{
	delete myCamera;
	myCamera = nullptr;

	delete myModel;
	myModel = nullptr;

	delete myParticle;
	myParticle = nullptr;
}

void DLLApp::Render()
{
	Prism::Engine::GetInstance()->Render();

	myDirectionalLight->Update();
	myDirectionalLightData[0].myDirection = myDirectionalLight->GetCurrentDir();
	myDirectionalLightData[0].myColor = myDirectionalLight->GetColor();

	if (myModel->GetInstance() != nullptr)
	{
		myModel->GetInstance()->UpdateDirectionalLights(myDirectionalLightData);
		myModel->GetInstance()->Render(*myCamera->GetCamera());
	}
	myParticle->Render(myCamera->GetCamera());

	Prism::DebugDrawer::GetInstance()->Render(*myCamera->GetCamera());
}

void DLLApp::Update()
{
	CU::TimerManager::GetInstance()->Update();
	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	CU::InputWrapper::GetInstance()->Update();
	LogicUpdate(deltaTime);
}

void DLLApp::LoadModel(const char* aModelFile, const char* aShaderFile)
{
	myModel->LoadModel(aModelFile, aShaderFile);
	myModelFile = aModelFile;
	myShaderFile = aShaderFile;
}

void DLLApp::LoadParticle(const char* aParticleFile)
{
	DL_ASSERT_EXP(myParticle != nullptr, "myParticle dont exists!");
	std::string particleFile = aParticleFile;
	myParticle->LoadParticle(particleFile);
}

void DLLApp::SetClearColor(CU::Vector4f& aClearColor)
{
	Prism::Engine::GetInstance()->SetClearColor({ aClearColor.x, aClearColor.y, aClearColor.z, aClearColor.w });
}

void DLLApp::SetCubeMap(const char* aCubeMapFile)
{
	Prism::EffectContainer::GetInstance()->SetCubeMap(aCubeMapFile);
	LoadModel(myModelFile.c_str(), myShaderFile.c_str());
}

void DLLApp::LogicUpdate(float aDeltaTime)
{
	if (GetActiveWindow()) {
		if (InputInstance->KeyIsPressed(DIK_LALT) && InputInstance->MouseIsPressed(0))
		{
			myCamera->Zoom(aDeltaTime, myMouseSensitivty);
		}
		if (InputInstance->KeyIsPressed(DIK_LALT) && InputInstance->MouseIsPressed(2))
		{
			myCamera->Pan(aDeltaTime, myMouseSensitivty);
		}
		if (InputInstance->KeyIsPressed(DIK_LALT) && InputInstance->MouseIsPressed(1))
		{
			myCamera->Rotate(aDeltaTime, myMouseSensitivty);
		}
		if (InputInstance->KeyDown(DIK_SPACE))
		{
			myCamera->ResetCamera();
		}
	}
	myModel->Update(aDeltaTime);
	myParticle->Update(aDeltaTime);
}

void DLLApp::SetupCubeMap()
{
	Prism::EffectContainer::GetInstance()->SetCubeMap("Data/Resource/Texture/CubeMap/T_cubemap_test.dds");
	EngineInstance->SetClearColor(CU::Vector4f(0.3f, 0.3f, 0.3f, 1.f));
}

void DLLApp::SetupInput()
{
	CU::InputWrapper::GetInstance()->Create(myPanelWindowHandler, GetModuleHandle(NULL),
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}

void DLLApp::SetParentWindow(Prism::SetupInfo &aWindowSetup)
{
	SetParent(myEngineWindowHandler, myPanelWindowHandler);
	SetWindowLong(myEngineWindowHandler, GWL_STYLE, WS_POPUP);
	SetWindowPos(myEngineWindowHandler, HWND_TOP, 0, 0,
		aWindowSetup.myScreenWidth, aWindowSetup.myScreenHeight, SWP_SHOWWINDOW);
}

void DLLApp::SetupLight()
{
	myDirectionalLight = new Prism::DirectionalLight();
	myDirectionalLight->SetDir(CU::Vector3f( 0.f, 0.f, -1.f ));
	myDirectionalLight->SetColor(CU::Vector4f(1.f, 1.f, 1.f, 1.f));

	memset(&myDirectionalLightData[0], 0, sizeof(Prism::DirectionalLightData) * NUMBER_OF_DIRECTIONAL_LIGHTS);
}

void DLLApp::SetDirectionalLightRotation(CU::Vector3f aRotation)
{
	myDirectionalLightRotation = aRotation;
	myDirectionalLight->SetDir(myDirectionalLightRotation);
}
