#pragma once

#include "LightStructs.h"

class Camera;
class Instance;

namespace Prism
{
	class PointLight
	{
	public:
		PointLight(unsigned int aGID, bool aAmbientOnly);
		~PointLight();
		void Render(const Camera& aCamera);
		void Update();

		const CU::Vector4<float>& GetColor() const;
		void SetColor(const CU::Vector4<float>& aVector);

		CU::Vector4<float> GetPosition() const;
		void SetPosition(const CU::Vector3<float>& aPosition);

		void SetRange(float aRange);
		float GetRange() const;

		bool GetAmbientOnly() const;

		const PointLightData& GetLightData() const;
	private:
		unsigned int myGID;
		CU::Vector4<float> myColor;
		CU::Matrix44<float> myOrientation;

		float myRange;
		bool myAmbientOnly;

		Instance* myLightMesh; 
		PointLightData myLightData;
	};

	inline const CU::Vector4<float>& PointLight::GetColor() const
	{
		return myColor;
	}

	inline void PointLight::SetColor(const CU::Vector4<float>& aColor)
	{
		myColor = aColor;
	}

	inline CU::Vector4<float> PointLight::GetPosition() const
	{
		return myOrientation.GetPos4();
	}

	inline void PointLight::SetPosition(const CU::Vector3<float>& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	inline void PointLight::SetRange(float aRange)
	{
		myRange = aRange;
	}

	inline float PointLight::GetRange() const
	{
		return myRange;
	}

	inline bool PointLight::GetAmbientOnly() const
	{
		return myAmbientOnly;
	}

	inline const PointLightData& PointLight::GetLightData() const
	{
		return myLightData;
	}
}

