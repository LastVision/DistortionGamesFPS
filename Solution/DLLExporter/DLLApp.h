#pragma once
#pragma warning(disable : 4005)
#include <Defines.h>
#include <InputWrapper.h>
#include <LightStructs.h>
#include <Scene.h>
#include <string>
#include <StaticArray.h>
#include <Vector.h>
#include <Windows.h>

class DLLCamera;
class DLLModel;
class DLLParticle;


namespace Prism
{
	class DirectionalLight;
	class Scene;
	class Room;
	class Instance;
	struct SetupInfo;
};

namespace CU
{
	class InputWrapper;
	class TimerManager;
}

class DLLApp
{
public:
	DLLApp(int* aHwnd, Prism::SetupInfo& aWindowSetup, WNDPROC aWindowProc);
	~DLLApp();

	void Render();
	void RenderScene();
	void Update();

	void RemoveActiveModel();

	void LoadModel(const char* aModelFile, const char* aShaderFile);
	void LoadParticle(const char* aParticleFile);

	void SetClearColor(CU::Vector4f& aClearColor);
	void SetCubeMap(const char* aCubeMapFile);

	void SetMouseSensitivty(float aValue);
	float GetMouseSensitivty() const;

	CU::Vector3f GetDirectionalLightRotation() const;
	void SetDirectionalLightRotation(CU::Vector3f aRotation);

	DLLCamera* GetCamera();
	DLLModel* GetModel();

private:
	void LogicUpdate(float aDeltaTime);

	bool HasMouseDeltaXMoved();
	bool HasMouseDeltaYMoved();

	void SetupInput();
	void SetupLight();
	void SetupCubeMap();
	void SetParentWindow(Prism::SetupInfo &aWindowSetup);

	DLLCamera* myCamera;
	DLLModel* myModel;
	DLLParticle* myParticle;
	Prism::Instance* myInstance;
	CU::TimerManager* myTimeManager;


	Prism::DirectionalLight* myDirectionalLight;
	CU::Vector3f myDirectionalLightRotation;

	float myMouseSensitivty;

	HWND myEngineWindowHandler;
	HWND myPanelWindowHandler;

	std::string myModelFile;
	std::string myShaderFile;

	Prism::Scene myScene;
	Prism::Room* myRoom;
};

inline bool DLLApp::HasMouseDeltaYMoved()
{
	return (CU::InputWrapper::GetInstance()->GetMouseDY() < myMouseSensitivty || CU::InputWrapper::GetInstance()->GetMouseDY() > myMouseSensitivty);
}

inline bool DLLApp::HasMouseDeltaXMoved()
{
	return (CU::InputWrapper::GetInstance()->GetMouseDX() < myMouseSensitivty || CU::InputWrapper::GetInstance()->GetMouseDX() > myMouseSensitivty);
}

inline float DLLApp::GetMouseSensitivty() const
{
	return myMouseSensitivty;
}
inline void DLLApp::SetMouseSensitivty(float val)
{
	myMouseSensitivty = val;
}

inline CU::Vector3f DLLApp::GetDirectionalLightRotation() const
{
	return myDirectionalLightRotation;
}

inline DLLCamera* DLLApp::GetCamera()
{
	return myCamera;
}
inline DLLModel* DLLApp::GetModel()
{
	return myModel;
}