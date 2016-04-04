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

	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, const CU::Vector3f& aSize, short reserverd);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, const CU::Vector3f& aDirection);

	Prism::ParticleEmitterInstance* myEmitter = nullptr;
	const std::string myParticleTypeString = "NONE";
	const CU::Vector3f myPosition = CU::Vector3f();
	const CU::Vector3f mySize = CU::Vector3f();
	const CU::Vector3f myDirection = CU::Vector3f();
	const int myEntityID = -1;
	const bool myShouldAlwaysShow = false;
	const bool myShouldKillEmitter = false;
	const bool myUseDirection = false;
	const float myEmitterLifeTime = 0.f;
	const float myRadius = 0.f;
};

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myEntityID(aEntityID)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myShouldAlwaysShow(aShouldAlwaysShow)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEmitterLifeTime(anEmitterLifeTime)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, float aRadius)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEmitterLifeTime(anEmitterLifeTime)
	, myRadius(aRadius)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, const CU::Vector3f& aSize)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEmitterLifeTime(anEmitterLifeTime)
	, mySize(aSize)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, bool aShouldKillEmitter, float aKilTime)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myShouldKillEmitter(aShouldKillEmitter)
	, myEmitterLifeTime(aKilTime)
{
}

inline EmitterMessage::EmitterMessage(Prism::ParticleEmitterInstance* anInstance, bool aKill)
	: Message(eMessageType::PARTICLE)
	, myEmitter(anInstance)
	, myShouldKillEmitter(aKill)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, const CU::Vector3f& aDirection)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myDirection(aDirection)
	, myUseDirection(true)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, const CU::Vector3f& aSize, short reserved)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, mySize(aSize)
{
	reserved;
}
