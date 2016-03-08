#include "stdafx.h"

#include <CommonHelper.h>

#include "PhysEntity.h"
#include <PxPhysics.h>
#include <pxphysicsapi.h>

#include "PhysicsInterface.h"
#include "PhysicsManager.h"
#include "wavefront.h"

#define MATERIAL_ID			0x01

namespace Prism
{
	PhysEntity::PhysEntity(const PhysEntityData& aPhysData
		, const CU::Matrix44<float>& aOrientation, const std::string& aFBXPath
		, Entity* aEntity)
		: myShapes(nullptr)
		, myEntity(aEntity)
	{
		for (int i = 0; i < 16; ++i)
		{
			my4x4Float[i] = aOrientation.myMatrix[i];
			myThread4x4Float[i] = aOrientation.myMatrix[i];
		}

		myPosition[0] = my4x4Float[12];
		myPosition[1] = my4x4Float[13];
		myPosition[2] = my4x4Float[14];

		physx::PxPhysics* core = PhysicsInterface::GetInstance()->GetManager()->GetCore();
		physx::PxMaterial* material = core->createMaterial(0.5, 0.5, 0.5);

		physx::PxReal density = 1.f;

		physx::PxMat44 matrix(my4x4Float);
		physx::PxTransform transform(matrix);

		myStaticBody = nullptr;
		myDynamicBody = nullptr;
		myPhysicsType = aPhysData.myPhysics;

		if (myPhysicsType == ePhysics::STATIC)
		{
			physx::PxTriangleMesh* mesh = GetPhysMesh(aFBXPath);

			myStaticBody = core->createRigidStatic(transform);
			myStaticBody->createShape(physx::PxTriangleMeshGeometry(mesh), *material);
			myStaticBody->setName("Tjohej");
			myStaticBody->userData = this;
			PhysicsInterface::GetInstance()->GetManager()->GetScene()->addActor(*myStaticBody);
		}
		else if (myPhysicsType == ePhysics::DYNAMIC)
		{
			physx::PxVec3 dimensions(
				aPhysData.myPhysicsMax.x - aPhysData.myPhysicsMin.x
				, aPhysData.myPhysicsMax.y - aPhysData.myPhysicsMin.y
				, aPhysData.myPhysicsMax.z - aPhysData.myPhysicsMin.z);
			physx::PxBoxGeometry geometry(dimensions / 2.f);
			myDynamicBody = physx::PxCreateDynamic(*core, transform, geometry, *material, density);
			myDynamicBody->setAngularDamping(0.75);
			myDynamicBody->setLinearVelocity(physx::PxVec3(0, 0, 0));
			myDynamicBody->userData = this;
			PhysicsInterface::GetInstance()->GetManager()->GetScene()->addActor(*myDynamicBody);

			physx::PxU32 nShapes = myDynamicBody->getNbShapes();
			myShapes = new physx::PxShape*[nShapes];
		}
		else if (myPhysicsType == ePhysics::PHANTOM)
		{
			physx::PxVec3 dimensions(
				aPhysData.myPhysicsMax.x - aPhysData.myPhysicsMin.x
				, aPhysData.myPhysicsMax.y - aPhysData.myPhysicsMin.y
				, aPhysData.myPhysicsMax.z - aPhysData.myPhysicsMin.z);
			physx::PxBoxGeometry geometry(dimensions / 2.f);
			myDynamicBody = physx::PxCreateDynamic(*core, transform, geometry, *material, density);
			myDynamicBody->setAngularDamping(0.75);
			myDynamicBody->setLinearVelocity(physx::PxVec3(0, 0, 0));
			myDynamicBody->userData = this;

			physx::PxU32 nShapes = myDynamicBody->getNbShapes();
			myShapes = new physx::PxShape*[nShapes];

			physx::PxShape* treasureShape;
			myDynamicBody->getShapes(&treasureShape, 1.f);

			treasureShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			treasureShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
			PhysicsInterface::GetInstance()->GetManager()->GetScene()->addActor(*myDynamicBody);
		}

		PhysicsInterface::GetInstance()->GetManager()->Add(this);
	}


	PhysEntity::~PhysEntity()
	{
		delete[] myShapes;
		myShapes = nullptr;
	}

	void PhysEntity::SwapOrientations()
	{
		std::swap(my4x4Float, myThread4x4Float);
	}

	float* PhysEntity::GetPosition()
	{
		DL_ASSERT("Not used? Not verified to work.");
		return &my4x4Float[12];
	}

	float* PhysEntity::GetOrientation()
	{
		return my4x4Float;
	}

	void PhysEntity::UpdateOrientation()
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant update Orientation on STATIC PhysEntities");

		physx::PxU32 nShapes = myDynamicBody->getNbShapes();
		myDynamicBody->getShapes(myShapes, nShapes);

		physx::PxTransform pT = physx::PxShapeExt::getGlobalPose(*myShapes[0], *myDynamicBody);
		physx::PxTransform graphicsTransform(pT.p, pT.q);
		physx::PxMat33 m = physx::PxMat33(graphicsTransform.q);

		myThread4x4Float[0] = m(0, 0);
		myThread4x4Float[1] = m(1, 0);
		myThread4x4Float[2] = m(2, 0);
		myThread4x4Float[3] = 0;
		
		myThread4x4Float[4] = m(0, 1);
		myThread4x4Float[5] = m(1, 1);
		myThread4x4Float[6] = m(2, 1);
		myThread4x4Float[7] = 0;
		
		myThread4x4Float[8] = m(0, 2);
		myThread4x4Float[9] = m(1, 2);
		myThread4x4Float[10] = m(2, 2);
		myThread4x4Float[11] = 0;
		
		myThread4x4Float[12] = graphicsTransform.p.x;
		myThread4x4Float[13] = graphicsTransform.p.y;
		myThread4x4Float[14] = graphicsTransform.p.z;
		myThread4x4Float[15] = 1;


		//myPosition[0] = graphicsTransform.p.x;
		//myPosition[1] = graphicsTransform.p.y;
		//myPosition[2] = graphicsTransform.p.z;
	}

	void PhysEntity::AddForce(const CU::Vector3<float>& aDirection, float aMagnitude)
	{
		//not implemented yet in threaded environment

		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

		//myDynamicBody->setLinearVelocity(physx::PxVec3(0, 0, 0));
		//myDynamicBody->addForce(physx::PxVec3(aDirection.x, aDirection.y, aDirection.z) * aMagnitude, physx::PxForceMode::eVELOCITY_CHANGE);
		PhysicsInterface::GetInstance()->AddForce(myDynamicBody, aDirection, aMagnitude);
	}

	void PhysEntity::SetVelocity(const CU::Vector3<float>& aVelocity)
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

		PhysicsInterface::GetInstance()->SetVelocity(myDynamicBody, aVelocity);
	}

	void PhysEntity::SetPosition(const CU::Vector3<float>& aPosition)
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

		PhysicsInterface::GetInstance()->SetPosition(myDynamicBody, aPosition);
	}

	physx::PxTriangleMesh* PhysEntity::GetPhysMesh(const std::string& aFBXPath)
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::STATIC, "Cant get phys mesh on DYNAMIC objects");

		std::string objPath(aFBXPath);
		std::string cowPath(aFBXPath);

		objPath[aFBXPath.size() - 3] = 'o';
		objPath[aFBXPath.size() - 2] = 'b';
		objPath[aFBXPath.size() - 1] = 'j';

		cowPath[aFBXPath.size() - 3] = 'c';
		cowPath[aFBXPath.size() - 2] = 'o';
		cowPath[aFBXPath.size() - 1] = 'w';

		physx::PxTriangleMesh* mesh = nullptr;
		WavefrontObj wfo;

		bool ok;

		if (CU::FileExists(cowPath) == false)
		{

			if (!wfo.loadObj(objPath.c_str(), false))
			{
				DL_ASSERT(CU::Concatenate("Error loading file: %s", objPath.c_str()));
			}

			physx::PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = wfo.mVertexCount;
			meshDesc.triangles.count = wfo.mTriCount;
			meshDesc.points.stride = sizeof(float) * 3;
			meshDesc.triangles.stride = sizeof(int) * 3;
			meshDesc.points.data = wfo.mVertices;
			meshDesc.triangles.data = wfo.mIndices;

			{
				physx::PxDefaultFileOutputStream stream(cowPath.c_str());
				ok = PhysicsInterface::GetInstance()->GetManager()->GetCooker()->cookTriangleMesh(meshDesc, stream);
			}
		}
		else
		{
			ok = true;
		}
		if (ok)
		{
			physx::PxDefaultFileInputData stream(cowPath.c_str());
			mesh = PhysicsInterface::GetInstance()->GetManager()->GetCore()->createTriangleMesh(stream);
		}

		return mesh;
	}

	void PhysEntity::RemoveFromScene()
	{
		//DL_ASSERT("Not impl yet");
		if (myPhysicsType == ePhysics::DYNAMIC || myPhysicsType == ePhysics::PHANTOM)
		{
			PhysicsInterface::GetInstance()->GetManager()->GetScene()->removeActor(*myDynamicBody);
		}
		else
		{
			PhysicsInterface::GetInstance()->GetManager()->GetScene()->removeActor(*myStaticBody);
		}
	}
}