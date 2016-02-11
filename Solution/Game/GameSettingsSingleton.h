#pragma once
class GameSettingsSingleton
{
public:
	static GameSettingsSingleton* GetInstance();
	static void Destroy();


	void ToggleShadows();
	bool GetShouldUseShadows() const;
private:
	GameSettingsSingleton();
	~GameSettingsSingleton();

	static GameSettingsSingleton* myInstance;

	bool myShouldUseShadows;
};

inline void GameSettingsSingleton::ToggleShadows()
{
	myShouldUseShadows = !myShouldUseShadows;
}

inline bool GameSettingsSingleton::GetShouldUseShadows() const
{
	return myShouldUseShadows;
}
