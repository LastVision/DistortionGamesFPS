#pragma once
#include <Subscriber.h>

class PlayerDirector;
class NeutralDirector;

class Tutorial : public Subscriber
{
public:
	Tutorial(const std::string& aXMLPath, const PlayerDirector* aPlayer, const CU::Matrix44<float>& aCameraOrientation
		, const NeutralDirector* aNeutral);
	~Tutorial();

	void Update(float aDeltaTime);

	void ReceiveMessage(const TutorialMessage& aMessage) override;

private:
	void CheckIfCompletedGlobalTutorial();

	enum class eAction
	{
		CLICK,
		ATTACK,
		SELECT,
		MOVE_CAMERA,
		ARTIFACT,
		RESOURCE_POINT,
		VICTORY_POINT,
		ENEMY_BASE,
	};

	struct Mission
	{
		Mission() {}
		Mission(Prism::Text* aText, eAction anAction, float aTime) 
			: myText(aText)
			, myAction(anAction)
			, myTime(aTime)
			, myIsDone(false) 
			, myIsActive(true)
		{}

		Prism::Text* myText;
		eAction myAction;
		float myTime;
		bool myIsDone;
		bool myIsActive;
	};

	eAction GetAction(const std::string& anAction) const;
	eAction GetSpecialAction(const std::string& anAction) const;

	void operator=(Tutorial&) = delete;

	const PlayerDirector* myPlayer;
	const NeutralDirector* myNeutral;
	int myPreviousNeutralCount;
	const CU::Matrix44<float>& myCameraOrientation;
	CU::Vector3<float> myPreviousCameraPosition;

	float myMaxTime;
	float myCurrentTime;

	CU::GrowingArray<Mission> myMissions;
	int myCurrentMission;
	bool myMissionComplete;
	bool myActive;

	Mission* myArtifactMission;
	Mission* myVictoryPointMission;
	Mission* myResourcePointMission;
	Mission* myEnemyBaseMission;
};

