#pragma once

namespace Prism
{
	class Effect;
	class Model;
	class ModelAnimated;

	class ModelProxy
	{
		friend class Instance;
	public:
		ModelProxy();

		void Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition);

		void SetModel(Model* aModel);
		void SetModelAnimated(ModelAnimated* aModel);

		Effect* GetEffect();
		void SetEffect(Effect* aEffect);

		bool IsLoaded() const;
		bool IsAnimated() const;
	private:
		Model* myModel;
		ModelAnimated* myModelAnimated;
	};

	inline bool ModelProxy::IsLoaded() const
	{
		if (myModel == nullptr && myModelAnimated == nullptr)
		{
			return false;
		}

		return true;
	}

	inline bool ModelProxy::IsAnimated() const
	{
		if (myModelAnimated == nullptr)
		{
			return false;
		}

		return true;
	}
}

