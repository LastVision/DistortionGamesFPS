#pragma once
#include "Matrix.h"
#include <string>

struct GUIBone
{
	GUIBone(){}
	GUIBone(const CU::Matrix44<float>* aBindMatrix, const CU::Matrix44<float>* aJointMatrix, int aJointID)
		: myBind(aBindMatrix)
		, myJoint(aJointMatrix)
	, myJointID(aJointID){}
	const CU::Matrix44<float>* myBind;
	const CU::Matrix44<float>* myJoint;
	int myJointID;

};