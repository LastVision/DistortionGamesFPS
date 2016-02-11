#pragma once

struct ActorComponentData
{
	float myMoveSpeed;
	float myVisionRange;
	float myAttackRange;
	float myAttackDamage;
	float myAttackRechargeTime;
	float myChaseDistance;
	float myChaseDistanceNeutral;
	float myRangerOneShotCoolDown = 0.f;
	bool myExistsInEntity = false;
};