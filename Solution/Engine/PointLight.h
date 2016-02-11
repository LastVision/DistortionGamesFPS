#pragma once

class Camera;
class Instance;

namespace Prism
{
	class PointLight
	{
	public:
		PointLight();
		void Initiate();
		void Render(Camera* aCamera);
		void Update();

		const CU::Vector4<float>& GetColor() const;
		void SetColor(const CU::Vector4<float>& aVector);

		const CU::Vector4<float>& GetPosition() const;
		void SetPosition(const CU::Vector4<float>& aPosition);

		void SetRange(float aRange);
		float GetRange() const;


		void PerformTransformation(const CU::Matrix44<float>& aTransformation);


	private:
		CU::Vector4<float> myColor;
		CU::Vector4<float> myPosition;
		CU::Vector4<float> myOriginalPosition;

		CU::Matrix44<float> myOrientation;

		float myRange;

		Instance* myInstance; 
		float myObjectCullingRadius;
	};
}

inline void Prism::PointLight::Update()
{
	myPosition = myOriginalPosition * myOrientation;
}

inline const CU::Vector4<float>& Prism::PointLight::GetColor() const
{
	return myColor;
}

inline void Prism::PointLight::SetColor(const CU::Vector4<float>& aColor)
{
	myColor = aColor;
}

inline const CU::Vector4<float>& Prism::PointLight::GetPosition() const
{
	return myPosition;
}

inline void Prism::PointLight::SetPosition(const CU::Vector4<float>& aPosition)
{
	myPosition = aPosition;
	myOriginalPosition = aPosition;
}

inline void Prism::PointLight::SetRange(float aRange)
{
	myRange = aRange;
}

inline float Prism::PointLight::GetRange() const
{
	return myRange;
}

inline void Prism::PointLight::PerformTransformation(const CU::Matrix44<float>& aTransformation)
{
	myOrientation = aTransformation * myOrientation;
}