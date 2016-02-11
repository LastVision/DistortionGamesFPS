using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using CSharpUtilities;

namespace EntityEditor
{
    class EntityReader
    {
        private String myFilePath = "";
        private Entity.EntityData myEntityData = new Entity.EntityData();
        private Entity.EntityListXML myEntityList = new Entity.EntityListXML();

        private XMLWrapperRead myXMLWrapper = new XMLWrapperRead();

        public Entity.EntityData LoadFile(String aFilePath, Form aParent)
        {
            myEntityData = new Entity.EntityData();
            if (aFilePath == "") return myEntityData;
            myFilePath = aFilePath;
            string entityListPath = StringUtilities.GetDataFolderPath(aFilePath) + "Script/LI_list_entity.xml";

            if (myEntityList.myPaths == null)
            {
                myEntityList.myPaths = new List<string>();
            }
            XmlDocument entityDoc = myXMLWrapper.Open(myFilePath);
            XmlNode rootElement = myXMLWrapper.FindFirstElement();
            XmlNode entityElement;
            entityElement = myXMLWrapper.FindFirstChildElement(rootElement);
            if (rootElement.Name != "root")
            {
                entityElement = rootElement;
            }
            myXMLWrapper.ReadAttribute(entityElement, "name", ref myEntityData.myName);
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(entityElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                ReadElement(e);
            }

            XmlDocument entityListDoc = myXMLWrapper.Open(entityListPath);
            rootElement = myXMLWrapper.FindFirstElement();
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(rootElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                ReadEntityListFile(e);
            }

            EntityEditorForm entityForm = (EntityEditorForm)aParent;
            entityForm.SetEntityList(myEntityList);

            return myEntityData;
        }

        public Entity.EntityListXML LoadFiles(string aCurrentFilePath)
        {
            if (myEntityList.myPaths == null)
            {
                myEntityList.myPaths = new List<string>();
            }

            myEntityData = new Entity.EntityData();
            if (aCurrentFilePath == "") return myEntityList;
            string entityListPath = StringUtilities.GetDataFolderPath(aCurrentFilePath) + "Script/LI_list_entity.xml";

            XmlDocument entityListDoc = myXMLWrapper.Open(entityListPath);
            XmlNode rootElement = myXMLWrapper.FindFirstElement();
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(rootElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                ReadEntityListFile(e);
            }
            return myEntityList;
        }

        private void ReadEntityListFile(XmlNode aNode)
        {
            if (aNode.Name == "path")
            {
                string path = "";
                myXMLWrapper.ReadAttribute(aNode, "src", ref path);

                if (myEntityList.myPaths.Contains(path) == false)
                {
                    myEntityList.myPaths.Add(path);
                }
            }
        }

        private void ReadElement(XmlNode aNode)
        {
            if (aNode.Name == "AIComponent")
            {
                myEntityData.myAIComponent.myIsActive = true;
                ReadAIComponent(aNode);
            }
            else if (aNode.Name == "BulletComponent")
            {
                myEntityData.myBulletComponent.myIsActive = true;
                ReadBulletComponent(aNode);
            }
            else if (aNode.Name == "CollisionComponent")
            {
                myEntityData.myCollisionComponent.myIsActive = true;
                ReadCollisionComponent(aNode);
            }
            else if (aNode.Name == "GraphicsComponent")
            {
                myEntityData.myGraphicsComponent.myIsActive = true;
                ReadGraphicsComponent(aNode);
            }
            else if (aNode.Name == "HealthComponent")
            {
                myEntityData.myHealthComponent.myIsActive = true;
                ReadHealthComponent(aNode);
            }
            else if (aNode.Name == "ParticleEmitterComponent")
            {
                myEntityData.myParticleEmitterComponent.myIsActive = true;
                ReadParticleEmitterComponent(aNode);
            }
            else if (aNode.Name == "PhysicsComponent")
            {
                myEntityData.myPhysicsComponent.myIsActive = true;
                ReadPhysicsComponent(aNode);
            }
            else if (aNode.Name == "PowerUpComponent")
            {
                myEntityData.myPowerUpComponent.myIsActive = true;
                ReadPowerUpComponent(aNode);
            }
            else if (aNode.Name == "ShootingComponent")
            {
                myEntityData.myShootingComponent.myIsActive = true;
                ReadShootingComponent(aNode);
            }
            else if (aNode.Name == "SoundComponent")
            {
                myEntityData.mySoundComponent.myIsActive = true;
                ReadSoundComponent(aNode);
            }
            else if (aNode.Name == "StreakEmitterComponent")
            {
                myEntityData.myStreakEmitterComponent.myIsActive = true;
                ReadStreakEmitterComponent(aNode);
            }
        }

        private void ReadPowerUpComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "Power")
                {
                    myXMLWrapper.ReadAttribute(e, "type", ref myEntityData.myPowerUpComponent.myType);
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myPowerUpComponent.myValue);
                    myXMLWrapper.ReadAttribute(e, "time", ref myEntityData.myPowerUpComponent.myTime);
                }
                if (e.Name == "WeaponUpgrade")
                {
                    myEntityData.myPowerUpComponent.myType = "WeaponUpgrade";
                    myXMLWrapper.ReadAttribute(e, "entityName", ref myEntityData.myPowerUpComponent.myUpgradedWeapon);
                    myXMLWrapper.ReadAttribute(e, "weaponID", ref myEntityData.myPowerUpComponent.myWeaponID);
                }
            }
        }

        private void ReadBulletComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "lifeTime")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myBulletComponent.myLifeTime);
                }
                if (e.Name == "damage")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myBulletComponent.myDamage);
                }
            }
        }

        private void ReadPhysicsComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "Weight")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myHealthComponent.myHealth);
                }
            }
        }

        private void ReadHealthComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "Health")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myHealthComponent.myHealth);
                }
            }
        }

        private void ReadGraphicsComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "Model")
                {
                    myXMLWrapper.ReadAttribute(e, "modelFile", ref myEntityData.myGraphicsComponent.myModelPath);
                    myXMLWrapper.ReadAttribute(e, "effectFile", ref myEntityData.myGraphicsComponent.myEffectPath);
                }
                else if (e.Name == "Scale")
                {
                    myXMLWrapper.ReadAttribute(e, "x", ref myEntityData.myGraphicsComponent.myScale.myX);
                    myXMLWrapper.ReadAttribute(e, "y", ref myEntityData.myGraphicsComponent.myScale.myY);
                    myXMLWrapper.ReadAttribute(e, "z", ref myEntityData.myGraphicsComponent.myScale.myZ);
                }
            }
        }

        private void ReadAIComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "Speed")
                {
                    myXMLWrapper.ReadAttribute(e, "min", ref myEntityData.myAIComponent.mySpeed.myX);
                    myXMLWrapper.ReadAttribute(e, "max", ref myEntityData.myAIComponent.mySpeed.myY);
                }
                else if (e.Name == "TimeToNextDecision")
                {
                    myXMLWrapper.ReadAttribute(e, "min", ref myEntityData.myAIComponent.myTimeToNextDecision.myX);
                    myXMLWrapper.ReadAttribute(e, "max", ref myEntityData.myAIComponent.myTimeToNextDecision.myY);
                }
                else if (e.Name == "FollowEntity")
                {
                    myXMLWrapper.ReadAttribute(e, "targetName", ref myEntityData.myAIComponent.myEntityToFollow);
                }
                else if (e.Name == "AIMode")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myAIComponent.myAIMode);
                }
                else if (e.Name == "AvoidanceDistance")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myAIComponent.myAvoidanceDistance);
                }
                else if (e.Name == "AvoidanceOffset")
                {
                    myXMLWrapper.ReadAttribute(e, "x", ref myEntityData.myAIComponent.myAvoidanceOffset.myX);
                    myXMLWrapper.ReadAttribute(e, "y", ref myEntityData.myAIComponent.myAvoidanceOffset.myY);
                    myXMLWrapper.ReadAttribute(e, "z", ref myEntityData.myAIComponent.myAvoidanceOffset.myZ);
                }
                else if (e.Name == "AITurnRate")
                {
                    myXMLWrapper.ReadAttribute(e, "value", ref myEntityData.myAIComponent.myAITurnRate);
                }
            }
        }

        private void ReadShootingComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "Weapon")
                {
                    myXMLWrapper.ReadAttribute(e, "type", ref myEntityData.myShootingComponent.myWeaponType);
                }
            }
        }

        private void ReadCollisionComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "CollisionSphere")
                {
                    myEntityData.myCollisionComponent.myHasSphere = true;
                    myXMLWrapper.ReadAttribute(e, "radius", ref myEntityData.myCollisionComponent.myRadius);
                }
            }
        }

        private void ReadParticleEmitterComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "path")
                {
                    myXMLWrapper.ReadAttribute(e, "src", ref myEntityData.myParticleEmitterComponent.myEmitterXML);
                }
            }
        }

        private void ReadSoundComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {

            }
        }

        private void ReadStreakEmitterComponent(XmlNode aNode)
        {
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(aNode); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                if (e.Name == "path")
                {
                    myXMLWrapper.ReadAttribute(e, "src", ref myEntityData.myStreakEmitterComponent.myEmitterXML);
                }
            }
        }

    }
}
