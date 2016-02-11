#pragma once
#include <Vector.h>
#include <GrowingArray.h>

class Level;
class XMLReader;

namespace Prism
{
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class Camera;
	class Terrain;
}

namespace GUI
{
	class Cursor;
}

namespace tinyxml2
{
	class XMLElement;
}

struct NavmeshCutBox
{
	NavmeshCutBox(const CU::Vector3f& aPosition, const CU::Vector3f& aExtend, const CU::Vector3f& aRotation);
	CU::GrowingArray<CU::Vector2<float>> GetCutMesh() const;
	CU::Vector3f myPosition;
	CU::Vector3f myExtend;
	CU::Vector3f myRotation;
};

class LevelFactory
{
public:
	LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera, GUI::Cursor* aCursor);
	~LevelFactory();

	Level* LoadLevel(const int& aID, bool aPauseModelLoader = true, eDifficulty aDifficulty = eDifficulty::NORMAL);
	Level* LoadCurrentLevel(bool aPauseModelLoader = true, eDifficulty aDifficulty = eDifficulty::NORMAL);
	Level* LoadNextLevel(eDifficulty aDifficulty);

	const volatile bool& IsLevelLoading() const;
	const int& GetLevelID() const;

	bool IsLastLevel() const;

	bool IsClean();
	void Cleanup();
private:
	void operator=(LevelFactory) = delete;

	void ReadLevelList(const std::string& aLevelListPath);
	void ReadLevel(const std::string& aLevelPath, std::string& aTutorialPathOut);
	void ReadLevelSetting(const std::string& aLevelPath);

	void LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadDirectionalLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadControlPoints(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadBases(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadCutBoxes(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadArtifacts(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadTerrain(const std::string& aLevelPath);
	void LoadParticles(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	Level* myCurrentLevel;
	Level* myOldLevel;

	Prism::Camera& myCamera;
	Prism::Terrain* myTerrain;

	CU::Vector4f myAmbientHue;
	CU::GrowingArray<Prism::DirectionalLight*> myDirectionalLights;
	CU::GrowingArray<Prism::PointLight*> myPointLights;
	CU::GrowingArray<Prism::SpotLight*> mySpotLights;

	CU::GrowingArray<NavmeshCutBox*> myCutBoxes;

	std::unordered_map<int, std::string> myLevelPaths;

	int myCurrentID;

	volatile bool myIsLoading;

	std::thread* myLoadLevelThread;

	GUI::Cursor* myCursor;


};

inline const volatile bool& LevelFactory::IsLevelLoading() const
{
	return myIsLoading;
}

inline const int& LevelFactory::GetLevelID() const
{
	return myCurrentID;
}

inline bool LevelFactory::IsClean()
{
	return myLoadLevelThread == nullptr;
}