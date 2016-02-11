using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CSharpUtilities;

namespace EntityEditor.Entity
{
    public struct EntityData
    {
        public string myName;

        public AIComponentData myAIComponent;
        public BulletComponentData myBulletComponent;
        public CollisionComponentData myCollisionComponent;
        public GraphicsComponentData myGraphicsComponent;
        public HealthComponentData myHealthComponent;
        public ParticleEmitterComponentData myParticleEmitterComponent;
        public PhysicsComponentData myPhysicsComponent;
        public PowerUpComponentData myPowerUpComponent;
        public ShootingComponentData myShootingComponent;
        public SoundComponentData mySoundComponent;
        public StreakEmitterComponentData myStreakEmitterComponent;
    }

    public struct EntityListXML
    {
        public List<string> myPaths;
    }

    public struct AIComponentData
    {
        public bool myIsActive;

        public Vector2<float> mySpeed;
        public Vector2<float> myTimeToNextDecision;
        public Vector3<float> myAvoidanceOffset;
        public float myAvoidanceDistance;
        public float myAITurnRate;
        public int myAIMode;
        public string myEntityToFollow;
    }

    public struct BulletComponentData
    {
        public bool myIsActive;

        public float myLifeTime;
        public float myDamage;
    }

    public struct CollisionComponentData
    {
        public bool myIsActive;

        public bool myHasSphere;
        public float myRadius;
    }

    public struct GraphicsComponentData
    {
        public Vector3<float> myScale;
        public bool myIsActive;

        public string myEffectPath;
        public string myModelPath;
    }

    public struct HealthComponentData
    {
        public bool myIsActive;

        public int myHealth;
    }

    public struct PhysicsComponentData
    {
        public bool myIsActive;

        public float myWeight;
    }

    public struct PowerUpComponentData
    {
        public bool myIsActive;

        public string myType;
        public float myValue;
        public float myTime;

        public string myUpgradedWeapon;
        public int myWeaponID;
    }

    public struct ParticleEmitterComponentData
    {
        public bool myIsActive;

        public string myEmitterXML;
    }

    public struct StreakEmitterComponentData
    {
        public bool myIsActive;

        public string myEmitterXML;
    }

    public struct ShootingComponentData
    {
        public bool myIsActive;

        public string myWeaponType;
    }

    public struct SoundComponentData
    {
        public bool myIsActive;

    }
}
