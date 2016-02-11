using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using CSharpUtilities;

namespace EntityEditor.Entity
{
    class EntityWriter
    {
        private String myFilePath = "";
        private Entity.EntityData myEntityData;
        private Entity.EntityListXML myEntityList;

        public void SaveFile(String aFilePath, Entity.EntityData aEntityData, Entity.EntityListXML aEntityList)
        {
            myFilePath = aFilePath;
            myEntityData = aEntityData;
            myEntityList = aEntityList;

            string entityListPath = StringUtilities.GetDataFolderPath(aFilePath) + "Script/LI_list_entity.xml";

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.OmitXmlDeclaration = true;
            settings.Indent = true;

            using (XmlWriter writer = XmlWriter.Create(myFilePath, settings))
            {
                WriteFile(writer);
            }

            using (XmlWriter writer = XmlWriter.Create(entityListPath, settings))
            {
                WriteEntityListFile(writer, myFilePath);
            }

        }

        private string ConvertPathToDataFolderPath(string aPath)
        {
            int dataTagIndex = aPath.IndexOf("Data");
            string convertedPath = aPath;
            convertedPath = aPath.Substring(dataTagIndex);
            convertedPath = convertedPath.Replace("\\", "/");

            return convertedPath;
        }

        private void WriteEntityListFile(XmlWriter aWriter, string aNewEntityPath)
        {
            aWriter.WriteStartElement("root");

            bool pathAlreadyExist = false;

            if (myEntityList.myPaths != null)
            {
                for (int i = 0; i < myEntityList.myPaths.Count; ++i)
                {
                    aWriter.WriteStartElement("path");
                    aWriter.WriteAttributeString("src", myEntityList.myPaths[i]);
                    aWriter.WriteEndElement();
                    if (myEntityList.myPaths[i] == ConvertPathToDataFolderPath(aNewEntityPath))
                    {
                        pathAlreadyExist = true;
                    }
                }
            }
            if(pathAlreadyExist == false)
            {
                aWriter.WriteStartElement("path");
                aWriter.WriteAttributeString("src", ConvertPathToDataFolderPath(aNewEntityPath));
                aWriter.WriteEndElement();
            }

            aWriter.WriteEndElement();
        }

        private void WriteFile(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("root");
            aWriter.WriteStartElement("Entity");
            aWriter.WriteAttributeString("name", myEntityData.myName);

            if (myEntityData.myAIComponent.myIsActive == true)
            {
                WriteAIComponent(aWriter);
            }
            if (myEntityData.myBulletComponent.myIsActive == true)
            {
                WriteBulletComponent(aWriter);
            }
            if (myEntityData.myCollisionComponent.myIsActive == true)
            {
                WriteCollisionComponent(aWriter);
            }
            if (myEntityData.myGraphicsComponent.myIsActive == true)
            {
                WriteGraphicsComponent(aWriter);
            }
            if (myEntityData.myHealthComponent.myIsActive == true)
            {
                WriteHealthComponent(aWriter);
            }
            if (myEntityData.myParticleEmitterComponent.myIsActive == true)
            {
                WriteParticleEmitterComponent(aWriter);
            }
            if (myEntityData.myPhysicsComponent.myIsActive == true)
            {
                WritePhysicsComponent(aWriter);
            }
            if (myEntityData.myPowerUpComponent.myIsActive == true)
            {
                WritePowerUpComponent(aWriter);
            }
            if (myEntityData.myShootingComponent.myIsActive == true)
            {
                WriteShootingComponent(aWriter);
            }
            if (myEntityData.mySoundComponent.myIsActive == true)
            {
                WriteSoundComponent(aWriter);
            }
            if (myEntityData.myStreakEmitterComponent.myIsActive == true)
            {
                WriteStreakEmitterComponent(aWriter);
            }

            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WritePowerUpComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("PowerUpComponent");
            if (myEntityData.myPowerUpComponent.myType == "WeaponUpgrade")
            {
                aWriter.WriteStartElement("WeaponUpgrade");
                aWriter.WriteAttributeString("entityName", myEntityData.myPowerUpComponent.myUpgradedWeapon);
                aWriter.WriteAttributeString("weaponID", myEntityData.myPowerUpComponent.myWeaponID.ToString());
                aWriter.WriteEndElement();
            }
            else
            {
                aWriter.WriteStartElement("Power");
                aWriter.WriteAttributeString("type", myEntityData.myPowerUpComponent.myType);
                aWriter.WriteAttributeString("value", myEntityData.myPowerUpComponent.myValue.ToString());
                aWriter.WriteAttributeString("time", myEntityData.myPowerUpComponent.myTime.ToString());
                aWriter.WriteEndElement();
            }
            aWriter.WriteEndElement();
        }

        private void WriteBulletComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("BulletComponent");
            aWriter.WriteStartElement("lifeTime");
            aWriter.WriteAttributeString("value", myEntityData.myBulletComponent.myLifeTime.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("damage");
            aWriter.WriteAttributeString("value", myEntityData.myBulletComponent.myDamage.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WritePhysicsComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("PhysicsComponent");
            aWriter.WriteStartElement("Weight");
            aWriter.WriteAttributeString("value", myEntityData.myPhysicsComponent.myWeight.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WriteGraphicsComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("GraphicsComponent");
            aWriter.WriteStartElement("Model");

            aWriter.WriteAttributeString("modelFile", myEntityData.myGraphicsComponent.myModelPath);
            aWriter.WriteAttributeString("effectFile", myEntityData.myGraphicsComponent.myEffectPath);

            aWriter.WriteEndElement();
            aWriter.WriteStartElement("Scale");
            aWriter.WriteAttributeString("x", myEntityData.myGraphicsComponent.myScale.myX.ToString());
            aWriter.WriteAttributeString("y", myEntityData.myGraphicsComponent.myScale.myY.ToString());
            aWriter.WriteAttributeString("z", myEntityData.myGraphicsComponent.myScale.myZ.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WriteAIComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("AIComponent");
            aWriter.WriteStartElement("Speed");
            aWriter.WriteAttributeString("min", myEntityData.myAIComponent.mySpeed.myX.ToString());
            aWriter.WriteAttributeString("max", myEntityData.myAIComponent.mySpeed.myY.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("TimeToNextDecision");
            aWriter.WriteAttributeString("min", myEntityData.myAIComponent.myTimeToNextDecision.myX.ToString());
            aWriter.WriteAttributeString("max", myEntityData.myAIComponent.myTimeToNextDecision.myY.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("FollowEntity");
            aWriter.WriteAttributeString("targetName", myEntityData.myAIComponent.myEntityToFollow);
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("AIMode");
            aWriter.WriteAttributeString("value", myEntityData.myAIComponent.myAIMode.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("AvoidanceDistance");
            aWriter.WriteAttributeString("value", myEntityData.myAIComponent.myAvoidanceDistance.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("AvoidanceOffset");
            aWriter.WriteAttributeString("x", myEntityData.myAIComponent.myAvoidanceOffset.myX.ToString());
            aWriter.WriteAttributeString("y", myEntityData.myAIComponent.myAvoidanceOffset.myY.ToString());
            aWriter.WriteAttributeString("z", myEntityData.myAIComponent.myAvoidanceOffset.myZ.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteStartElement("AITurnRate");
            aWriter.WriteAttributeString("value", myEntityData.myAIComponent.myAITurnRate.ToString());
            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WriteShootingComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("ShootingComponent");
            aWriter.WriteStartElement("Weapon");

            aWriter.WriteAttributeString("type", myEntityData.myShootingComponent.myWeaponType);

            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WriteCollisionComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("CollisionComponent");

            if (myEntityData.myCollisionComponent.myHasSphere == true)
            {
                aWriter.WriteStartElement("CollisionSphere");
                aWriter.WriteAttributeString("radius", myEntityData.myCollisionComponent.myRadius.ToString());
                aWriter.WriteEndElement();
            }

            aWriter.WriteEndElement();
        }

        private void WriteHealthComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("HealthComponent");

            if (myEntityData.myCollisionComponent.myHasSphere == true)
            {
                aWriter.WriteStartElement("Health");
                aWriter.WriteAttributeString("value", myEntityData.myHealthComponent.myHealth.ToString());
                aWriter.WriteEndElement();
            }

            aWriter.WriteEndElement();
        }

        private void WriteParticleEmitterComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("ParticleEmitterComponent");
            aWriter.WriteStartElement("Path");
            aWriter.WriteAttributeString("src", myEntityData.myParticleEmitterComponent.myEmitterXML);
            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private void WriteSoundComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("SoundComponent");
            aWriter.WriteEndElement();
        }

        private void WriteStreakEmitterComponent(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("StreakEmitterComponent");
            aWriter.WriteStartElement("Path");
            aWriter.WriteAttributeString("src", myEntityData.myStreakEmitterComponent.myEmitterXML);
            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

    }
}
