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
    public class WeaponWriter
    {
        private String myFilePath = "";
        private Entity.WeaponListXml myWeaponListXml;
        private Entity.WeaponData myWeaponData;

        private Entity.BulletListXml myBulletListXml;
        private Entity.BulletData myBulletData;

        public void SaveWeaponFile(String aFilePath, Entity.WeaponData aWeaponData, Entity.WeaponListXml aWeaponList)
        {
            myFilePath = aWeaponData.myFilePath;
            myWeaponData = aWeaponData;
            myWeaponListXml = aWeaponList;

            string dataFolder = StringUtilities.GetDataFolderPath(aFilePath);
            dataFolder = dataFolder.Replace("Data/", "");
            string weaponListPath = dataFolder + "Data/Script/LI_list_weapon.xml";

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.OmitXmlDeclaration = true;
            settings.Indent = true;

            using (XmlWriter writer = XmlWriter.Create(dataFolder + myFilePath, settings))
            {
                WriteWeaponFile(writer);
            }

            using (XmlWriter writer = XmlWriter.Create(weaponListPath, settings))
            {
                WriteWeaponListFile(writer, myFilePath);
            }

        }

        private void WriteWeaponListFile(XmlWriter aWriter, string aFilePath)
        {
            aWriter.WriteStartElement("root");

            bool pathAlreadyExist = false;

            if (myWeaponListXml.myPaths != null)
            {
                for (int i = 0; i < myWeaponListXml.myPaths.Count; ++i)
                {
                    aWriter.WriteStartElement("path");
                    aWriter.WriteAttributeString("src", myWeaponListXml.myPaths[i]);
                    aWriter.WriteEndElement();
                    if (myWeaponListXml.myPaths[i] == ConvertPathToDataFolderPath(aFilePath))
                    {
                        pathAlreadyExist = true;
                    }
                }
            }
            if (pathAlreadyExist == false)
            {
                aWriter.WriteStartElement("path");
                aWriter.WriteAttributeString("src", ConvertPathToDataFolderPath(aFilePath));
                aWriter.WriteEndElement();
            }

            aWriter.WriteEndElement();
        }

        private void WriteWeaponFile(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("root");
            aWriter.WriteStartElement("Weapon");
            aWriter.WriteAttributeString("name", myWeaponData.myType);

            aWriter.WriteStartElement("cooldown");
            aWriter.WriteAttributeString("value", myWeaponData.myCooldown.ToString());
            aWriter.WriteEndElement();

            aWriter.WriteStartElement("spread");
            aWriter.WriteAttributeString("value", myWeaponData.mySpread.ToString());
            aWriter.WriteEndElement();

            aWriter.WriteStartElement("bulletsPerShot");
            aWriter.WriteAttributeString("value", myWeaponData.myNumberOfBulletsPerShot.ToString());
            aWriter.WriteEndElement();

            aWriter.WriteStartElement("position");
            aWriter.WriteAttributeString("x", myWeaponData.myPosition.myX.ToString());
            aWriter.WriteAttributeString("y", myWeaponData.myPosition.myY.ToString());
            aWriter.WriteAttributeString("z", myWeaponData.myPosition.myZ.ToString());
            aWriter.WriteEndElement();

            aWriter.WriteStartElement("bullet");
            aWriter.WriteAttributeString("type", myWeaponData.myBulletType);
            aWriter.WriteEndElement();

            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        public void SaveBulletFile(String aFilePath, Entity.BulletData aBulletData, Entity.BulletListXml aBulletList)
        {
            myFilePath = aFilePath;
            myBulletData = aBulletData;
            myBulletListXml = aBulletList;

            string dataFolder = StringUtilities.GetDataFolderPath(aFilePath);
            dataFolder = dataFolder.Replace("Data/", "");
            string projectileListPath = dataFolder + "Data/Script/LI_list_projectile.xml";

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.OmitXmlDeclaration = true;
            settings.Indent = true;

            using (XmlWriter writer = XmlWriter.Create(myFilePath, settings))
            {
                WriteBulletFile(writer);
            }

            using (XmlWriter writer = XmlWriter.Create(projectileListPath, settings))
            {
                WriteBulletListFile(writer, myFilePath);
            }
        }

        private void WriteBulletListFile(XmlWriter aWriter, string aFilePath)
        {
            aWriter.WriteStartElement("root");

            bool pathAlreadyExist = false;

            if (myBulletListXml.myPaths != null)
            {
                for (int i = 0; i < myBulletListXml.myPaths.Count; ++i)
                {
                    aWriter.WriteStartElement("path");
                    aWriter.WriteAttributeString("src", myBulletListXml.myPaths[i]);
                    aWriter.WriteEndElement();
                    if (myBulletListXml.myPaths[i] == ConvertPathToDataFolderPath(aFilePath))
                    {
                        pathAlreadyExist = true;
                    }
                }
            }
            if (pathAlreadyExist == false)
            {
                aWriter.WriteStartElement("path");
                aWriter.WriteAttributeString("src", ConvertPathToDataFolderPath(aFilePath));
                aWriter.WriteEndElement();
            }

            aWriter.WriteEndElement();
        }

        private void WriteBulletFile(XmlWriter aWriter)
        {
            aWriter.WriteStartElement("root");
            aWriter.WriteStartElement("Projectile");
            aWriter.WriteAttributeString("type", myBulletData.myType);

            aWriter.WriteStartElement("Entity");
            aWriter.WriteAttributeString("name", myBulletData.myEntityType);
            aWriter.WriteEndElement();

            aWriter.WriteStartElement("maxAmount");
            aWriter.WriteAttributeString("value", myBulletData.myMaxAmount.ToString());
            aWriter.WriteEndElement();

            aWriter.WriteStartElement("speed");
            aWriter.WriteAttributeString("value", myBulletData.mySpeed.ToString());
            aWriter.WriteEndElement();

            aWriter.WriteEndElement();
            aWriter.WriteEndElement();
        }

        private string ConvertPathToDataFolderPath(string aPath)
        {
            int dataTagIndex = aPath.IndexOf("Data");
            string convertedPath = aPath;
            convertedPath = aPath.Substring(dataTagIndex);
            convertedPath = convertedPath.Replace("\\", "/");

            return convertedPath;
        }
    }
}
