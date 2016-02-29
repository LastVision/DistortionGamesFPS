#pragma warning(disable : 4005)

#include "DLLApp.h"
#include "DLLCamera.h"
#include "DLLExport.h"
#include "DLLModel.h"
#include <Engine.h>
#include <SetupInfo.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DLLApp* locDLLApplication;
Prism::SetupInfo locWindowSetup;

void StartEngine(int* aHwnd)
{
	locDLLApplication = new DLLApp(aHwnd, locWindowSetup, WndProc);
}

void SetupWindow(int aWidth, int aHeight)
{
	locWindowSetup.myMSAACount = 4;
	locWindowSetup.myScreenHeight = aHeight;
	locWindowSetup.myScreenWidth = aWidth;
	locWindowSetup.myWindowed = true;
}

void Render()
{
	locDLLApplication->Render();
}

void Update()
{
	locDLLApplication->Update();
}

void RotateObjectAtX(float aSpeed)
{
	CU::Vector3f rotationSpeed(locDLLApplication->GetModel()->GetAutoRotationSpeed());
	rotationSpeed.x = aSpeed;
	locDLLApplication->GetModel()->SetAutoRotationSpeed(rotationSpeed);
}

void RotateObjectAtY(float aSpeed)
{
	CU::Vector3f rotationSpeed(locDLLApplication->GetModel()->GetAutoRotationSpeed());
	rotationSpeed.y = aSpeed;
	locDLLApplication->GetModel()->SetAutoRotationSpeed(rotationSpeed);
}

void RotateObjectAtZ(float aSpeed)
{
	CU::Vector3f rotationSpeed(locDLLApplication->GetModel()->GetAutoRotationSpeed());
	rotationSpeed.z = aSpeed;
	locDLLApplication->GetModel()->SetAutoRotationSpeed(rotationSpeed);
}

void SetRotateObjectAtX(float aAngle)
{
	CU::Vector3f rotationAngle(locDLLApplication->GetModel()->GetManualRotationAngle());
	rotationAngle.x = aAngle;
	locDLLApplication->GetModel()->SetManualRotationAngle(rotationAngle);
}

void SetRotateObjectAtY(float aAngle)
{
	CU::Vector3f rotationAngle(locDLLApplication->GetModel()->GetManualRotationAngle());
	rotationAngle.y = aAngle;
	locDLLApplication->GetModel()->SetManualRotationAngle(rotationAngle);
}

void SetRotateObjectAtZ(float aAngle)
{
	CU::Vector3f rotationAngle(locDLLApplication->GetModel()->GetManualRotationAngle());
	rotationAngle.z = aAngle;
	locDLLApplication->GetModel()->SetManualRotationAngle(rotationAngle);
}

void SetMouseSensitivty(float aValue)
{
	locDLLApplication->SetMouseSensitivty(aValue);
}

float GetMouseSensitivty()
{
	return locDLLApplication->GetMouseSensitivty();
}

void SetCameraZoomSpeed(float aValue)
{
	locDLLApplication->GetCamera()->SetZoomSpeed(aValue);
}

float GetCameraZoomSpeed()
{
	return locDLLApplication->GetCamera()->GetZoomSpeed();
}

void SetCameraMovementSpeed(float aValue)
{
	locDLLApplication->GetCamera()->SetMovementSpeed(aValue);
}

float GetCameraMovementSpeed()
{
	return locDLLApplication->GetCamera()->GetMovementSpeed();
}

void SetCameraRotationSpeed(float aValue)
{
	locDLLApplication->GetCamera()->SetRotationSpeed(aValue);
}

float GetCameraRotationSpeed()
{
	return locDLLApplication->GetCamera()->GetRotationSpeed();
}

void DirectionaLightRotateX(float aAngle)
{
	CU::Vector3<float> rotatedDirection(locDLLApplication->GetDirectionalLightRotation());
	rotatedDirection.x = aAngle;
	locDLLApplication->SetDirectionalLightRotation(rotatedDirection);
}

void DirectionaLightRotateY(float aAngle)
{
	CU::Vector3<float> rotatedDirection(locDLLApplication->GetDirectionalLightRotation());
	rotatedDirection.y = aAngle;
	locDLLApplication->SetDirectionalLightRotation(rotatedDirection);
}

void DirectionaLightRotateZ(float aAngle)
{
	CU::Vector3<float> rotatedDirection(locDLLApplication->GetDirectionalLightRotation());
	rotatedDirection.z = aAngle;
	locDLLApplication->SetDirectionalLightRotation(rotatedDirection);
}

float GetDirectionaLightXRotation()
{
	return locDLLApplication->GetDirectionalLightRotation().x;
}

float GetDirectionaLightYRotation()
{
	return locDLLApplication->GetDirectionalLightRotation().y;
}

float GetDirectionaLightZRotation()
{
	return locDLLApplication->GetDirectionalLightRotation().z;
}

void LoadModel(const char* aModelFile, const char* aShaderFile)
{
	locDLLApplication->LoadModel(aModelFile, aShaderFile);
}

void LoadParticle(const char* aParticleFile)
{
	locDLLApplication->LoadParticle(aParticleFile);
	DL_DEBUG("Loaded Particle %s", aParticleFile);
}

void SetClearColor(float aRChannel, float aGChannel, float aBChannel, float aAChannel)
{
	locDLLApplication->SetClearColor(CU::Vector4f(aRChannel, aGChannel, aBChannel, aAChannel));
}

void SetCubeMap(const char* aCubeMapFile) 
{
	DL_DEBUG("Stuff happend here first!");
	locDLLApplication->SetCubeMap(aCubeMapFile);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}