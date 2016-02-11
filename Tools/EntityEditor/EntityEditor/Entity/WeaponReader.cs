using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using CSharpUtilities;

namespace EntityEditor.Entity
{
    public class WeaponReader
    {
        private List<WeaponData> myWeaponData = new List<WeaponData>();
        private List<BulletData> myBulletData = new List<BulletData>();
        private WeaponListXml myWeaponPaths;
        private BulletListXml myBulletPaths;

        private WeaponData myNewWeaponData;
        private BulletData myNewBulletData;

        private XMLWrapperRead myXMLWrapper = new XMLWrapperRead();

        public void LoadWeapons(string aWeaponListPath)
        {
            if (aWeaponListPath == "") return;

            if (File.Exists(aWeaponListPath) == false) MessageBox.Show("Error file not found.");

            if (myWeaponPaths.myPaths == null)
            {
                myWeaponPaths.myPaths = new List<string>();
            }

            myWeaponData.Clear();
            myWeaponPaths.myPaths.Clear();
            XmlDocument weaponListDoc = myXMLWrapper.Open(aWeaponListPath);
            XmlNode rootElement = myXMLWrapper.FindFirstElement();
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(rootElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                ReadWeaponList(e);
            }


            for(int i = 0; i < myWeaponPaths.myPaths.Count; ++i)
            {
                string dataPath = StringUtilities.GetDataFolderPath(aWeaponListPath);
                dataPath = dataPath.Replace("Data/", "");
                XmlDocument weaponDoc = myXMLWrapper.Open(dataPath + myWeaponPaths.myPaths[i]);
                rootElement = myXMLWrapper.FindFirstElement();
                XmlNode weaponElement = myXMLWrapper.FindFirstChildElement(rootElement);
                if(rootElement.Name != "root") 
                {
                    weaponElement = rootElement;
                }
                myXMLWrapper.ReadAttribute(weaponElement, "name", ref myNewWeaponData.myType);
                for (XmlNode e = myXMLWrapper.FindFirstChildElement(weaponElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
                {
                    ReadWeaponData(e);
                }
                if(myNewWeaponData.myNumberOfBulletsPerShot == 0)
                {
                    myNewWeaponData.myNumberOfBulletsPerShot = 1;
                }
                myNewWeaponData.myFilePath = myWeaponPaths.myPaths[i];
                myWeaponData.Add(myNewWeaponData);
            }
        }

        private void ReadWeaponData(XmlNode aNode)
        {
            if (aNode.Name == "cooldown")
            {
                myXMLWrapper.ReadAttribute(aNode, "value", ref myNewWeaponData.myCooldown);
            }
            else if (aNode.Name == "spread")
            {
                myXMLWrapper.ReadAttribute(aNode, "value", ref myNewWeaponData.mySpread);
            }
            else if (aNode.Name == "bulletsPerShot")
            {
                myXMLWrapper.ReadAttribute(aNode, "value", ref myNewWeaponData.myNumberOfBulletsPerShot);
            }
            else if (aNode.Name == "position")
            {
                myXMLWrapper.ReadAttribute(aNode, "x", ref myNewWeaponData.myPosition.myX);
                myXMLWrapper.ReadAttribute(aNode, "y", ref myNewWeaponData.myPosition.myY);
                myXMLWrapper.ReadAttribute(aNode, "z", ref myNewWeaponData.myPosition.myZ);
            }
            else if (aNode.Name == "bullet")
            {
                myXMLWrapper.ReadAttribute(aNode, "type", ref myNewWeaponData.myBulletType);
            }
        }

        private void ReadWeaponList(XmlNode aNode)
        {
            if (aNode.Name == "path")
            {
                string path = "";
                myXMLWrapper.ReadAttribute(aNode, "src", ref path);
                if (myWeaponPaths.myPaths.Contains(path) == false)
                {
                    myWeaponPaths.myPaths.Add(path);
                }
            }
        }

        public List<WeaponData> GetWeaponData()
        {
            return myWeaponData;
        }

        public WeaponListXml GetWeaponListXml()
        {
            return myWeaponPaths;
        }

        public void LoadBullets(string aBulletListPath)
        {
            if (aBulletListPath == "") return;

            if (myBulletPaths.myPaths == null)
            {
                myBulletPaths.myPaths = new List<string>();
            }

            myBulletData.Clear();
            myBulletPaths.myPaths.Clear();
            XmlDocument bulletListDoc = myXMLWrapper.Open(aBulletListPath);
            XmlNode rootElement = myXMLWrapper.FindFirstElement();
            for (XmlNode e = myXMLWrapper.FindFirstChildElement(rootElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
            {
                ReadBulletList(e);
            }

            for (int i = 0; i < myBulletPaths.myPaths.Count; ++i)
            {
                string dataPath = StringUtilities.GetDataFolderPath(aBulletListPath);
                dataPath = dataPath.Replace("Data/", "");
                XmlDocument bulletDoc = myXMLWrapper.Open(dataPath + myBulletPaths.myPaths[i]);
                rootElement = myXMLWrapper.FindFirstElement();
                XmlNode bulletElement = myXMLWrapper.FindFirstChildElement(rootElement);
                if (rootElement.Name != "root")
                {
                    bulletElement = rootElement;
                }
                myXMLWrapper.ReadAttribute(bulletElement, "type", ref myNewBulletData.myType);
                for (XmlNode e = myXMLWrapper.FindFirstChildElement(bulletElement); e != null; e = myXMLWrapper.FindNextSiblingElement(e))
                {
                    ReadBulletData(e);
                }
                myNewBulletData.myFilePath = myBulletPaths.myPaths[i];
                myBulletData.Add(myNewBulletData);
            }
        }

        private void ReadBulletData(XmlNode aNode)
        {
            if (aNode.Name == "Entity")
            {
                myXMLWrapper.ReadAttribute(aNode, "name", ref myNewBulletData.myEntityType);
                if (myNewBulletData.myEntityType != "")
                {
                    myXMLWrapper.ReadAttribute(aNode, "type", ref myNewBulletData.myEntityType);
                }
            }
            else if (aNode.Name == "maxAmount")
            {
                myXMLWrapper.ReadAttribute(aNode, "value", ref myNewBulletData.myMaxAmount);
            }
            else if (aNode.Name == "speed")
            {
                myXMLWrapper.ReadAttribute(aNode, "value", ref myNewBulletData.mySpeed);
            }
        }

        private void ReadBulletList(XmlNode aNode)
        {
            if (aNode.Name == "path")
            {
                string path = "";
                myXMLWrapper.ReadAttribute(aNode, "src", ref path);
                if (myBulletPaths.myPaths.Contains(path) == false)
                {
                    myBulletPaths.myPaths.Add(path);
                }
            }
        }

        public List<BulletData> GetBulletData()
        {
            return myBulletData;
        }

        public BulletListXml GetBulletListXml()
        {
            return myBulletPaths;
        }
    }
}
