#pragma once

namespace Prism
{
	class DirectionalLight
	{
	public:
		void Update();

		CU::Vector4<float> GetCurrentDir() const;
		const CU::Vector4<float>& GetColor() const;
		const CU::Vector3<float>& GetDir() const;

		void PerformRotation(const CU::Matrix33<float>& aOrientation);

		void SetColor(const CU::Vector4<float>& aVector);
		void SetDir(const CU::Vector3<float>& aVector);

	private:
		CU::Vector4<float> myColor;
		CU::Vector4<float> myDirection;
		CU::Vector3<float> myOriginalDirection;
		CU::Matrix33<float> myOrientation;
	};
}

inline void Prism::DirectionalLight::Update()
{
	CU::Vector3f newDir = myOriginalDirection * myOrientation;

	myDirection = CU::Vector4<float>(newDir, 0);
}

inline CU::Vector4<float> Prism::DirectionalLight::GetCurrentDir() const
{
	return myDirection;
}

inline const CU::Vector4<float>& Prism::DirectionalLight::GetColor() const
{
	return myColor;
}

inline const CU::Vector3<float>& Prism::DirectionalLight::GetDir() const
{
	return myOriginalDirection;
}

 
inline void Prism::DirectionalLight::PerformRotation(const CU::Matrix33<float>& aOrientation)
{
	myOrientation = aOrientation * myOrientation;
}

 
inline void Prism::DirectionalLight::SetColor(const CU::Vector4<float>& aVector)
{
	myColor = aVector;
}

inline void Prism::DirectionalLight::SetDir(const CU::Vector3<float>& aVector)
{
	myOriginalDirection = aVector;
	myDirection = CU::Vector4<float>(aVector, 0);
}