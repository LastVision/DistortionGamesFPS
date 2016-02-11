#pragma once
#include "StreakInstance.h"
#include "IndexBufferWrapper.h"
#include <Matrix.h>
#include <Vector.h>
#include <GrowingArray.h>
#include "StreakEmitterData.h"
#include "VertexBufferWrapper.h"

#define MAX_NR_OF_StreakS_IN_EMITTER 4096

namespace Prism
{
	class Camera;


	class StreakEmitterInstance
	{
	public:
		StreakEmitterInstance(StreakEmitterData& anEmitter);
		~StreakEmitterInstance();
		void Render(Camera* aCamera);
		void Update(float aDeltaTime);
		const CU::Matrix44f& GetOrientation() const;
		const CU::Vector3f& GetPosition() const;
		//void PerformRotation(const CU::Matrix33<float>& anOrientation);
		void PerformTransformation(const CU::Matrix44f& anOrientation);
		void SetOrientation(const CU::Matrix44f& anOrientation);
		void SetPosition(const CU::Vector3f& aPosition);

		void Reset();

	private:
		void operator=(StreakEmitterInstance&) = delete;
		void CreateIndexBuffer();
		void CreateVertexBuffer();
		void EmitterUpdate(float aDeltaTime);
		void EmitStreak();
		void StreakUpdate(float aDeltaTime);
		void UpdateVertexBuffer();

		CU::Matrix33<float> myEmissionOrientation;
		StreakEmitterData myEmitter;
		float myGravity;
		IndexBufferWrapper myIndexBufferWrapper;
		CU::Matrix44f myOrientation;
		CU::GrowingArray<unsigned int> myStreakIndices;
		CU::GrowingArray<StreakInstance> myStreaks;
		CU::Vector3f myPosition;
		float myProcessTime;
		float myTimeSinceLastSpawn;
		VertexBufferWrapper myVertexBufferWrapper;

		ID3D11BlendState* myBlendStateNoBlend;
	};


	inline const CU::Matrix44f& StreakEmitterInstance::GetOrientation() const
	{
		return myOrientation;
	}

	inline const CU::Vector3f& StreakEmitterInstance::GetPosition() const
	{
		return myPosition;
	}

	//inline void StreakEmitterInstance::PerformRotation(const CU::Matrix33<float>& anOrientation)
	//{
	//	myOrientation *= anOrientation;
	//}

	inline void StreakEmitterInstance::PerformTransformation(const CU::Matrix44f& anOrientation)
	{
		myOrientation *= anOrientation;
	}

	inline void StreakEmitterInstance::SetOrientation(const CU::Matrix44f& anOrientation)
	{
		myOrientation = anOrientation;
		myPosition = myOrientation.GetPos();
	}

	inline void StreakEmitterInstance::SetPosition(const CU::Vector3f& aPosition)
	{
		myOrientation.SetPos(aPosition);
		myPosition = aPosition;
	}
}