using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CSharpUtilities;

namespace ParticleEditor.ParticleData
{
    public struct ParticleData
    {
        public Vector3<float> myEmitterSize;
        public Vector3<float> myStartColor;
        public Vector3<float> myEndColor;

        public Vector2<float> myRotationMinMax;
        public Vector2<float> myParticleMinMax;

        public float mySpeedMultiplier;
        public float myParticleLifeTime;
        public float myEmitterLifeTime;
        public float myEmissionRate;
        public float myEmissionRateDelta;
        public float myParticleAlphaDelta;
        public float myParticleSizeDelta;
        public float myParticleAlphaStart;

        public int myParticlesPerEmitt;

        public string myShaderPath;
        public string myTexturePath;

        public bool myAcitveAtStart;
        public bool myUseEmitterLifeTime;
    }
}
