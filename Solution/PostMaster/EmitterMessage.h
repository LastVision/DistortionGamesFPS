#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

namespace Prism
{
	class Camera;
	class ParticleEmitterInstance;
}

struct EmitterMessage : public Message
{
	EmitterMessage(const std::string& aParticleType, int aEntityID);
	EmitterMessage(Prism::ParticleEmitterInstance* anInstance, bool aKill);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition);
	EmitterMessage(const std::string& aParticleType, bool aShouldKillEmitter, float aKilTime);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow);
	EmitterMessage(const std::string& aParticleType, bool aIsArtifact, int aEntityID);
	EmitterMessage(const std::string& aParticleType, Prism::Camera* aCamera, bool aShouldAlwaysShow);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, const CU::Vector3f& aSize);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, float aRadius);

	Prism::ParticleEmitterInstance* myEmitter;
	Prism::Camera* myCamera;
	const std::string myParticleTypeString;
	const CU::Vector3f myPosition;
	const CU::Vector3f mySize;
	const int myEntityID;
	const bool myShouldAlwaysShow;
	const bool myShouldKillEmitter;
	const bool myIsArtifact;
	const float myEmitterLifeTime;
	const float myRadius;
};

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myEmitter(nullptr)
	, myCamera(nullptr)
	, myParticleTypeString(aParticleType)
	, mySize({ 0.f, 0.f, 0.f })
	, myEntityID(aEntityID)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myEmitterLifeTime(0.f)
	, myRadius(0.f)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myEmitter(nullptr)
	, myCamera(nullptr)
	, myParticleTypeString(aParticleType)
	, mySize({ 0.f, 0.f, 0.f })
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myRadius(0.f)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myCamera(nullptr)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(aShouldAlwaysShow)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myEmitter(nullptr)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, bool aIsArtifact, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition({0.f,0.f,0.f})
	, myEntityID(aEntityID)
	, myCamera(nullptr)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
	, myShouldKillEmitter(false)
	, myIsArtifact(aIsArtifact)
	, myEmitter(nullptr)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, Prism::Camera* aCamera, bool aShouldAlwaysShow)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition({0.f,0.f,0.f})
	, myEntityID(-1)
	, myCamera(aCamera)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(aShouldAlwaysShow)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myEmitter(nullptr)
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
	, myCamera(nullptr)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myEmitter(nullptr)
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
	, myCamera(nullptr)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myEmitter(nullptr)
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
	, myCamera(nullptr)
	, myShouldKillEmitter(false)
	, myIsArtifact(false)
	, myEmitter(nullptr)
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
	, myCamera(nullptr)
	, myShouldKillEmitter(aShouldKillEmitter)
	, myIsArtifact(false)
	, myEmitter(nullptr)
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
	, myCamera(nullptr)
	, myShouldKillEmitter(aKill)
	, myIsArtifact(false)
	, myEmitter(anInstance)
{
}
