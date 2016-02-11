#pragma once
#include <Matrix.h>
#include <GameEnum.h>
namespace Prism
{
	class Instance;
}

class DLLModel
{
public:
	DLLModel();
	~DLLModel();

	void Update(float aDeltaTime);
	void LoadModel(const char* aModelFile, const char* aEffectFile);

	void SetAutoRotationSpeed(const CU::Vector3f& aRotationSpeed);
	const CU::Vector3f& GetAutoRotationSpeed() const;

	void SetManualRotationAngle(const CU::Vector3f& aRotationAngle);
	const CU::Vector3f& GetManualRotationAngle() const;

	Prism::Instance* GetInstance() const;
private:
	void WatchFiles(const char* aModelFile, const char* aShaderFile);

	void AutoRotate(float aDeltaTime);
	void ManualRotate();

	CU::Matrix44f myOrientation;
	Prism::Instance* myInstance;

	CU::Vector3f myAutoRotationSpeed;
	CU::Vector3f myManualRotationAngle;
};

inline void DLLModel::SetAutoRotationSpeed(const CU::Vector3f& aRotationSpeed)
{
	myAutoRotationSpeed = aRotationSpeed;
}

inline const CU::Vector3f& DLLModel::GetAutoRotationSpeed() const
{
	return myAutoRotationSpeed;
}

inline void DLLModel::SetManualRotationAngle(const CU::Vector3f& aRotationAngle)
{
	myManualRotationAngle = aRotationAngle;
	ManualRotate();
}
inline const CU::Vector3f& DLLModel::GetManualRotationAngle() const
{
	return myManualRotationAngle;
}

inline Prism::Instance* DLLModel::GetInstance() const
{
	return myInstance;
}