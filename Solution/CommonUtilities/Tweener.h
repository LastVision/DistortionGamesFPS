#pragma once


enum class eTweenType
{
	LINEAR,
	QUADRIC,
	QUADRIC_HALF,
	CUBIC,
	CUBIC_HALF,
	QUARTIC,
	QUARTIC_HALF,
	QUINTIC,
	QUINTIC_HALF,
	SINUS,
	SINUS_HALF,
	EXPONENTIAL,
	EXPONENTIAL_HALF,
	CIRCULAR,
	CIRCULAR_HALF,
	BOUNCE,
};

template <typename T>
class Tweener
{
public:
	Tweener();
	~Tweener();

	T DoTween(float aDelta, T aStart, T aChangeInValue, float aDuration, eTweenType aType);

private:
	T Linear(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Quadric(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T QuadricHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Cubic(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T CubicHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Quartic(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T QuarticHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Quintic(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T QuinticHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Sinus(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T SinusHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Exponential(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T ExponentialHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Circular(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T CircularHalf(float aDelta, T aStart, T aChangeInValue, float aDuration);
	T Bounce(float aDelta, T aStart, T aChangeInValue, float aDuration);
};

template <typename T>
Tweener<T>::Tweener()
{
}

template <typename T>
Tweener<T>::~Tweener()
{

}

template <typename T>
T Tweener<T>::DoTween(float aDelta, T aStart, T aChangeInValue, float aDuration, eTweenType aType)
{
	switch (aType)
	{
	case eTweenType::LINEAR:
		return Linear(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::QUADRIC:
		return Quadric(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::QUADRIC_HALF:
		return QuadricHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::CUBIC:
		return Cubic(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::CUBIC_HALF:
		return CubicHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::QUARTIC:
		return Quadric(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::QUARTIC_HALF:
		return QuarticHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::QUINTIC:
		return Quintic(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::QUINTIC_HALF:
		return QuinticHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::SINUS:
		return Sinus(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::SINUS_HALF:
		return SinusHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::EXPONENTIAL:
		return Exponential(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::EXPONENTIAL_HALF:
		return ExponentialHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::CIRCULAR:
		return Circular(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::CIRCULAR_HALF:
		return CircularHalf(aDelta, aStart, aChangeInValue, aDuration);
		break;
	case eTweenType::BOUNCE:
		return Bounce(aDelta, aStart, aChangeInValue, aDuration);
		break;
	}

	return aStart;
}




//*************************************
//******** Private Functions **********
//* Implementation of Tween Functions *
//*************************************


template <typename T>
T Tweener<T>::Linear(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	return aChangeInValue * (aDelta / aDuration) + aStart;
}

template <typename T>
T Tweener<T>::Quadric(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration;
	return aChangeInValue * aDelta * aDelta + aStart;
}

template <typename T>
T Tweener<T>::QuadricHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration / 2.f;

	if (aDelta < 1.f) return aChangeInValue / 2.f * aDelta * aDelta + aStart;

	aDelta--;

	return (-aChangeInValue / 2.f) * (aDelta * (aDelta - 2.f) - 1.f) + aStart;
}

template <typename T>
T Tweener<T>::Cubic(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration;
	return aChangeInValue * aDelta * aDelta * aDelta + aStart;
}

template <typename T>
T Tweener<T>::CubicHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration / 2.f;

	if (aDelta < 1.f) return aChangeInValue / 2.f * aDelta * aDelta * aDelta + aStart;

	aDelta -= 2;

	return aChangeInValue / 2.f * (aDelta * aDelta * aDelta + 2.f) + aStart;
}

template <typename T>
T Tweener<T>::Quartic(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration;
	return aChangeInValue * aDelta * aDelta * aDelta * aDelta + aStart;
}

template <typename T>
T Tweener<T>::QuarticHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration / 2.f;

	if (aDelta < 1.f) return aChangeInValue / 2.f * aDelta * aDelta * aDelta * aDelta + aStart;

	aDelta -= 2;

	return (-aChangeInValue / 2.f) * (aDelta * aDelta * aDelta * aDelta - 2.f) + aStart;
}

template <typename T>
T Tweener<T>::Quintic(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration;
	return aChangeInValue * aDelta * aDelta * aDelta * aDelta * aDelta + aStart;
}

template <typename T>
T Tweener<T>::QuinticHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration / 2.f;

	if (aDelta < 1.f) return aChangeInValue / 2.f * aDelta * aDelta * aDelta * aDelta * aDelta + aStart;

	aDelta -= 2;

	return (aChangeInValue / 2.f) * (aDelta * aDelta * aDelta * aDelta * aDelta + 2.f) + aStart;
}

template <typename T>
T Tweener<T>::Sinus(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	return aChangeInValue * sinf(aDelta / aDuration * static_cast<float>((M_PI / 2.f))) + aStart;
}

template <typename T>
T Tweener<T>::SinusHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	return (-aChangeInValue / 2.f) * (cosf(static_cast<float>(M_PI) * aDelta / aDuration) - 1.f) + aStart;
}

template <typename T>
T Tweener<T>::Exponential(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	return aChangeInValue * pow(2.f, 10.f * (aDelta / aDuration - 1.f)) + aStart;
}

template <typename T>
T Tweener<T>::ExponentialHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration / 2.f;

	if (aDelta < 1.f) return (aChangeInValue / 2.f) * pow(2.f, 10.f * (aDelta - 1.f)) + aStart;

	--aDelta;

	return (aChangeInValue / 2.f) * (-pow(2.f, -10.f * aDelta) + 2.f) + aStart;
}

template <typename T>
T Tweener<T>::Circular(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration;

	return -aChangeInValue * (sqrt(1.f - aDelta * aDelta) - 1.f) + aStart;
}

template <typename T>
T Tweener<T>::CircularHalf(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	aDelta /= aDuration / 2.f;

	if (aDelta < 1.f) return (-aChangeInValue / 2.f) * (sqrt(1.f - aDelta * aDelta) - 1.f) + aStart;

	--aDelta;

	return (aChangeInValue / 2.f) * (sqrt(1.f - aDelta * aDelta) + 1.f) + aStart;
}

template <typename T>
T Tweener<T>::Bounce(float aDelta, T aStart, T aChangeInValue, float aDuration)
{
	if ((aDelta /= aDuration) < (1 / 2.75f))
	{
		return aChangeInValue * (7.5625f * aDelta * aDelta) + aStart;
	}
	else if (aDelta < (2 / 2.75f)) 
	{
		float postFix = aDelta -= (1.5f / 2.75f);
		return aChangeInValue * (7.5625f * (postFix)* aDelta + .75f) + aStart;
	}
	else if (aDelta < (2.5 / 2.75)) {
		float postFix = aDelta -= (2.25f / 2.75f);
		return aChangeInValue * (7.5625f * (postFix)* aDelta + .9375f) + aStart;
	}
	else {
		float postFix = aDelta -= (2.625f / 2.75f);
		return aChangeInValue * (7.5625f * (postFix)* aDelta + .984375f) + aStart;
	}
}