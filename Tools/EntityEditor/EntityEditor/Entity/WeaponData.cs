using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CSharpUtilities;

namespace EntityEditor.Entity
{
    public struct BulletData
    {
        public string myType;
        public string myEntityType;
        public int myMaxAmount;
        public float mySpeed;

        public string myFilePath;
    }

    public struct BulletListXml
    {
        public List<string> myPaths;
    }

    public struct WeaponData
    {
        public float myCooldown;
        public Vector3<float> myPosition;
        public float mySpread;
        public int myNumberOfBulletsPerShot;
        public string myBulletType;
        public string myType;

        public string myFilePath;
    }

    public struct WeaponListXml
    {
        public List<string> myPaths;
    }
}
