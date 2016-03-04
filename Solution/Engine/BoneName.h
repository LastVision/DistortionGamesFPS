#pragma once
#include "Matrix.h"
#include <string>

struct GUIBone
{
	GUIBone(){}
	GUIBone(CU::Matrix44<float>* aBindMatrix, CU::Matrix44<float>* aJointMatrix, int aJointID)
		: myBind(aBindMatrix)
		, myJoint(aJointMatrix)
	, myJointID(aJointID){}
	CU::Matrix44<float>* myBind;
	CU::Matrix44<float>* myJoint;
	int myJointID;

};