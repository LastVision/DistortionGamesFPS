using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using CSharpUtilities;

namespace ParticleEditor.ParticleData
{
    public class ParticleReader
    {
        private CSharpUtilities.XMLWrapperRead myReader;
        private ParticleData myNewParticleData = new ParticleData();

        public ParticleReader() 
        {

        }

        public void LoadFile(string aFilePath)
        {
            XmlDocument particleDoc = myReader.Open(aFilePath);

            XmlNode rootNode = myReader.FindFirstElement();

            for(XmlNode e = myReader.FindFirstChildElement(rootNode); e != null; e = myReader.FindNextSiblingElement(e))
            {
                ReadFile(e);
            }
        }

        private void ReadFile(XmlNode aNode)
        {
            switch (aNode.Name)
            {
                case "Shader":
                    myReader.ReadAttribute(aNode, "filepath", ref myNewParticleData.myShaderPath);
                    break;
                case "Texture":
                    myReader.ReadAttribute(aNode, "filepath", ref myNewParticleData.myTexturePath);
                    break;
                case "EmitterSize":
                    ReadVector(aNode, ref myNewParticleData.myEmitterSize);
                    break;
                case "ParticleMinRotation":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myRotationMinMax.myX);
                    break;
                case "ParticleMaxRotation":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myRotationMinMax.myY);
                    break;
                case "ParticleSpeedMultiplier":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.mySpeedMultiplier);
                    break;
                case "ParticleStartColor":
                    ReadVector(aNode, ref myNewParticleData.myStartColor);
                    break;
                case "ParticleEndColor":
                    ReadVector(aNode, ref myNewParticleData.myEndColor);
                    break;
                case "ParticleLifeTime":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticleLifeTime);
                    break;
                case "EmitterLifeTime":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myEmitterLifeTime);
                    break;
                case "UseEmitterLifeTime":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myUseEmitterLifeTime);
                    break;
                case "EmissionRate":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myEmissionRate);
                    break;
                case "ParticlePerEmitt":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticlesPerEmitt);
                    break;
                case "EmissionRateDelta":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myEmissionRateDelta);
                    break;
                case "ParticleMaxScale":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticleMinMax.myX);
                    break;
                case "ParticleMinScale":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticleMinMax.myY);
                    break;
                case "ParticleAlphaDelta":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticleAlphaDelta);
                    break;
                case "ParticleSizeDelta":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticleSizeDelta);
                    break;
                case "ParticleAlphaStart":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myParticleAlphaStart);
                    break;
                case "ActiveAtStart":
                    myReader.ReadAttribute(aNode, "value", ref myNewParticleData.myAcitveAtStart);
                    break;
                default:
                    DL_Debug.GetInstance.DL_ErrorMessage("The element " + aNode.Name
                        + " is not supported. Please speak to Daniel Carlsson about it");
                    break;
            }
        }

        private void ReadVector(XmlNode aNode, ref Vector3<float> aVector)
        {
            myReader.ReadAttribute(aNode, "x", ref aVector.myX);
            myReader.ReadAttribute(aNode, "y", ref aVector.myY);
            myReader.ReadAttribute(aNode, "z", ref aVector.myZ);
        }

    }
}
