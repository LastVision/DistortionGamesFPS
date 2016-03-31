#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

namespace Prism
{
	class ParticleEmitterInstance;
}

struct EmitterMessage : public Message
{
	EmitterMessage(const std::string& aParticleType, int aEntityID);
	EmitterMessage(Prism::ParticleEmitterInstance* anInstance, bool aKill);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition);
	EmitterMessage(const std::string& aParticleType, bool aShouldKillEmitter, float aKilTime);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, const CU::Vector3f& aSize);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, float aRadius);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, const CU::Vector3f& aDirection);

	Prism::ParticleEmitterInstance* myEmitter;
	const std::string myParticleTypeString;
	const CU::Vector3f myPosition;
	const CU::Vector3f mySize;
	const CU::Vector3f myDirection;
	const int myEntityID;
	const bool myShouldAlwaysShow;
	const bool myShouldKillEmitter;
	const bool myUseDirection;
	const float myEmitterLifeTime;
	const float myRadius;
};

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myEmitter(nullptr)
	, myParticleTypeString(aParticleType)
	, mySize({ 0.f, 0.f, 0.f })
	, myEntityID(aEntityID)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myUseDirection(false)
	, myEmitterLifeTime(0.f)
	, myRadius(0.f)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myEmitter(nullptr)
	, myParticleTypeString(aParticleType)
	, mySize({ 0.f, 0.f, 0.f })
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myUseDirection(false)
	, myRadius(0.f)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(aShouldAlwaysShow)
	, myShouldKillEmitter(false)
	, myUseDirection(false)
	, myEmitter(nullptr)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(anEmitterLifeTime)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myUseDirection(false)
	, myEmitter(nullptr)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, float aRadius)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(anEmitterLifeTime)
	, myRadius(aRadius)
	, mySize({ 0.f, 0.f, 0.f })
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myUseDirection(false)
	, myEmitter(nullptr)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, const CU::Vector3f& aSize)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(anEmitterLifeTime)
	, mySize(aSize)
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myUseDirection(false)
	, myEmitter(nullptr)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, bool aShouldKillEmitter, float aKilTime)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition({ 0.f, 0.f, 0.f })
	, myEntityID(-1)
	, myEmitterLifeTime(aKilTime)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(aShouldKillEmitter)
	, myUseDirection(false)
	, myEmitter(nullptr)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(Prism::ParticleEmitterInstance* anInstance, bool aKill)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString("")
	, myPosition({ 0.f, 0.f, 0.f })
	, myEntityID(-1)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(aKill)
	, myUseDirection(false)
	, myEmitter(anInstance)
	, myDirection(CU::Vector3f())
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, const CU::Vector3f& aDirection)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myUseDirection(true)
	, myEmitter(nullptr)
	, myDirection(aDirection)
{
}
