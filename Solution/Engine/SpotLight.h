#pragma once

#include "LightStructs.h"
#include "MathHelper.h"
class Camera;
class Instance;


namespace Prism
{
	class SpotLight
	{
	public:
		SpotLight(unsigned int aGID, bool aShouldDoInstance = true);
		virtual ~SpotLight();

		virtual void Render(const Camera& aCamera);
		virtual void Update();

		const CU::Vector4<float>& GetColor() const;
		void SetColor(const CU::Vector4<float>& aVector);

		CU::Vector4<float> GetPosition() const;
		void SetPosition(const CU::Vector4<float>& aPosition);

		CU::Vector4<float> GetDir() const;
		void SetDir(const CU::Vector3<float>& aVector);

		void SetRange(float aRange);
		float GetRange() const;

		void SetCone(float aCone);
		float GetCone() const;


		void SetAngle(float anAngleInRad);
		float GetAngle() const;

		void PerformTransformation(const CU::Matrix44<float>& aTransformation);

		const SpotLightData& GetLightData() const;

	protected:
		CU::Matrix44<float> myOrientation;
		float myRange;
		float myAngleInRad;
		Instance* myLightMesh;
		CU::Vector4<float> myPosition;

	private:
		unsigned int myGID;
		CU::Vector4<float> myDirection;
		CU::Vector4<float> myColor;

		float myCone;

		SpotLightData myLightData;
	};

	inline void SpotLight::Update()
	{
		myLightData.myColor = GetColor();
		myLightData.myCone = GetCone();
		myLightData.myAngle = GetAngle();
		myLightData.myCosAngle = cos(GetAngle());
		myLightData.myDirection = GetDir();
		myLightData.myPosition = GetPosition();
		myLightData.myRange = GetRange();
	}

	inline const CU::Vector4<float>& SpotLight::GetColor() const
	{
		return myColor;
	}

	inline void SpotLight::SetColor(const CU::Vector4<float>& aColor)
	{
		myColor = aColor;
	}

	inline CU::Vector4<float> SpotLight::GetPosition() const
	{
		return myPosition;
	}

	inline void SpotLight::SetPosition(const CU::Vector4<float>& aPosition)
	{
		myPosition = aPosition;
	}

	inline CU::Vector4<float> SpotLight::GetDir() const
	{
		return CU::Vector4<float>(myOrientation.GetForward(), 0.f);
	}

	inline void SpotLight::SetDir(const CU::Vector3<float>& aVector)
	{
		myDirection = CU::Vector4<float>(aVector, 0.f);
	}

	inline void SpotLight::SetRange(float aRange)
	{
		myRange = aRange;
		if (myRange < 0.f)
		{
			myRange = 0.f;
		}
	}

	inline float SpotLight::GetRange() const
	{
		return myRange;
	}

	inline void SpotLight::SetCone(float aCone)
	{
		myCone = aCone;
		if (myCone < 0.01f)
		{
			myCone = 0.01f;
		}
	}

	inline float SpotLight::GetCone() const
	{
		return myCone;
	}

	inline void SpotLight::SetAngle(float anAngleInRad)
	{
		myAngleInRad = anAngleInRad;
	}

	inline float SpotLight::GetAngle() const
	{
		return myAngleInRad;
	}

	inline void SpotLight::PerformTransformation(const CU::Matrix44<float>& aTransformation)
	{
		myOrientation = myOrientation * aTransformation;
	}

	inline const SpotLightData& SpotLight::GetLightData() const
	{
		return myLightData;
	}

	
}