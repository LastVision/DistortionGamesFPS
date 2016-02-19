#pragma once
#include <string>

namespace Prism
{
	class Model;
	class ModelAnimated;
	class Animation;
	class IModelFactory
	{
	public:
		virtual ~IModelFactory(){};

		virtual Model* LoadModel(const std::string& aFilePath) = 0;
		virtual ModelAnimated* LoadAnimatedModel(const std::string& aFilePath) = 0;
		virtual Animation* LoadAnimation(const std::string& aFilePath) = 0;

	protected:
		IModelFactory(){};
	};
}