using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Windows.Forms;
using CSharpUtilities.Components;

namespace EntityEditor.Panels
{
    public class WeaponDataComponent : BaseComponent
    {
        private NumericTextComponent myCooldown;
        private NumericTextComponent mySpread;
        private NumericTextComponent myBulletsPerShot;

        private Vector3Component myPosition;

        public WeaponDataComponent(Point aLocation, Size aSize)
            : base(aLocation, aSize, "")
        {
        }

        protected override void InitializeComponents(string aText)
        {
            myCooldown = new NumericTextComponent(new Point(myLocation.X, myLocation.Y), mySize, "Cooldown");

            mySpread = new NumericTextComponent(new Point(myLocation.X, myLocation.Y + mySize.Height + 10), mySize, "Spread");

            myBulletsPerShot = new NumericTextComponent(new Point(myLocation.X, myLocation.Y + ((mySize.Height * 2) + 20)), mySize, "Bullet/Shot");

            myPosition = new Vector3Component(new Point(myLocation.X, myLocation.Y + ((mySize.Height * 3) + 30)),
                new Size(mySize.Width, 13), "Position");
        }

        public override void BindToPanel(Panel aPanel)
        {
            myCooldown.BindToPanel(aPanel);
            mySpread.BindToPanel(aPanel);
            myBulletsPerShot.BindToPanel(aPanel);
            myPosition.BindToPanel(aPanel);
        }

        public override void Show()
        {
            myCooldown.Show();
            mySpread.Show();
            myBulletsPerShot.Show();
            myPosition.Show();
        }

        public override void Hide()
        {
            myCooldown.Hide();
            mySpread.Hide();
            myBulletsPerShot.Hide();
            myPosition.Hide();
        }

        public TextBox GetCooldown()
        {
            return myCooldown.GetTextBox();
        }

        public TextBox GetSpread()
        {
            return mySpread.GetTextBox();
        }

        public TextBox GetBulletPerShot()
        {
            return myBulletsPerShot.GetTextBox();
        }

        public Vector3Component GetPosition()
        {
            return myPosition;
        }
    }

    public class BulletDataComponent : BaseComponent
    {
        private NumericTextComponent myEntityType;
        private NumericTextComponent myMaxAmount;
        private NumericTextComponent mySpeed;

        public BulletDataComponent(Point aLocation, Size aSize) : base(aLocation, aSize, "")
        {

        }

        protected override void InitializeComponents(string aText)
        {
            myEntityType = new NumericTextComponent(myLocation, mySize, "Entity Type", false);
            myMaxAmount = new NumericTextComponent(new Point(myLocation.X, myLocation.Y + (mySize.Height) + 10), mySize, "Max Bullet");
            mySpeed = new NumericTextComponent(new Point(myLocation.X, myLocation.Y + ((mySize.Height * 2) + 20)), mySize, "Speed ");
        }

        public override void BindToPanel(Panel aPanel)
        {
            myEntityType.BindToPanel(aPanel);
            myMaxAmount.BindToPanel(aPanel);
            mySpeed.BindToPanel(aPanel);
        }

        public override void Show()
        {
            myEntityType.Show();
            myMaxAmount.Show();
            mySpeed.Show();
        }

        public override void Hide()
        {
            myEntityType.Hide();
            myMaxAmount.Hide();
            mySpeed.Hide();
        }

        public string GetEntityType()
        {
            return myEntityType.GetTextBox().Text;
        }

        public void SetEntityType(string aType)
        {
            myEntityType.GetTextBox().Text = aType;
        }

        public TextBox GetEntityTypeTextBox()
        {
            return myEntityType.GetTextBox();
        }

        public int GetMaxAmount()
        {
            return int.Parse(myMaxAmount.GetTextBox().Text);
        }

        public void SetMaxAmount(int aMaxAmount)
        {
            myMaxAmount.GetTextBox().Text = aMaxAmount.ToString();
        }

        public TextBox GetMaxAmountTextBox()
        {
            return myMaxAmount.GetTextBox();
        }

        public float GetSpeed()
        {
            return float.Parse(mySpeed.GetTextBox().Text);
        }

        public void SetSpeed(float aSpeed)
        {
            mySpeed.GetTextBox().Text = aSpeed.ToString();
        }

        public TextBox GetSpeedTextBox()
        {
            return mySpeed.GetTextBox();
        }
    }

    public class ShootingComponentPanel : BasePanel
    {
        private Entity.ShootingComponentData myShootingComponent;
        private Entity.WeaponData myWeaponData;
        private Entity.BulletData myBulletData;

        private Entity.WeaponReader myWeaponReader = new Entity.WeaponReader();
        private Entity.WeaponWriter myWeaponWriter = new Entity.WeaponWriter();

        private List<Entity.WeaponData> myLoadedWeapons;
        private List<Entity.BulletData> myLoadedBullets;

        private DropDownComponent myWeaponTypes;
        private DropDownComponent myBulletTypes;

        private WeaponDataComponent myWeaponDataStats;
        private BulletDataComponent myBulletDataStats;
        private Button mySaveButton = new Button();

        private string myDataFolder = "Data/";

        public ShootingComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Shooting Component";
        }

        public void Load(Entity.ShootingComponentData aShootingComponentData)
        {
            myShootingComponent = aShootingComponentData;
            if (myShootingComponent.myWeaponType != null && myShootingComponent.myWeaponType != "")
            {
                if (myWeaponTypes.GetDropDown().Items.Contains(myShootingComponent.myWeaponType) == false) return;
                for (int i = 0; i < myWeaponTypes.GetDropDown().Items.Count; ++i)
                {
                    if (myShootingComponent.myWeaponType == (string)myWeaponTypes.GetDropDown().Items[i])
                    {
                        myWeaponTypes.GetDropDown().SelectedIndex = i;
                        myWeaponData = myLoadedWeapons[i];
                        break;
                    }
                }
                LoadSettings();
            }
            myHasLoadedComponent = true;
        }

        protected override void IntitalizeControls()
        {
            myWeaponReader.LoadWeapons("Data/Script/LI_list_weapon.xml");
            myWeaponReader.LoadBullets("Data/Script/LI_list_projectile.xml");

            myWeaponTypes = new DropDownComponent(new Point(Location.X, Location.Y + 10), new Size(245, 10), "Weapon");
            myWeaponTypes.GetDropDown().SelectedIndexChanged += new EventHandler(WeaponDropDown_SelectIndex_Changed);
            myWeaponTypes.BindToPanel(this);
            myWeaponTypes.Show();

            myWeaponDataStats = new WeaponDataComponent(new Point(Location.X, Location.Y + 40), new Size(245, 13));
            myWeaponDataStats.BindToPanel(this);
            myWeaponDataStats.Show();

            myBulletTypes = new DropDownComponent(new Point(Location.X, Location.Y + 130), new Size(245, 10), "Bullet");
            myBulletTypes.GetDropDown().SelectedIndexChanged += new EventHandler(BulletDropDown_SelectIndex_Changed);
            myBulletTypes.BindToPanel(this);
            myBulletTypes.Show();

            myBulletDataStats = new BulletDataComponent(new Point(Location.X, Location.Y + 160), new Size(245, 13));
            myBulletDataStats.BindToPanel(this);
            myBulletDataStats.Show();

            myLoadedWeapons = myWeaponReader.GetWeaponData();
            for (int i = 0; i < myLoadedWeapons.Count; ++i)
            {
                myWeaponTypes.AddItem(myLoadedWeapons[i].myType);
            }

            myLoadedBullets = myWeaponReader.GetBulletData();
            for (int i = 0; i < myLoadedBullets.Count; ++i)
            {
                myBulletTypes.AddItem(myLoadedBullets[i].myType);
            }

            mySaveButton.Text = "Save";
            mySaveButton.Location = new Point(Location.X, Location.Y + 230);
            mySaveButton.Size = new Size(100, 20);
            mySaveButton.Click += new EventHandler(this.SaveButton_Click);
            mySaveButton.Show();
            this.Controls.Add(mySaveButton);
        }

        public void ReloadXML(string aDataPath)
        {
            myDataFolder = aDataPath;
            ReloadWeaponXML();
        }

        private void ReloadWeaponXML()
        {
            myWeaponReader.LoadWeapons(myDataFolder + "Script/LI_list_weapon.xml");
            myWeaponReader.LoadBullets(myDataFolder + "Script/LI_list_projectile.xml");

            myLoadedWeapons = myWeaponReader.GetWeaponData();
            myWeaponTypes.GetDropDown().Items.Clear();
            for (int i = 0; i < myLoadedWeapons.Count; ++i)
            {
                myWeaponTypes.AddItem(myLoadedWeapons[i].myType);
            }

            myLoadedBullets = myWeaponReader.GetBulletData();
            myBulletTypes.GetDropDown().Items.Clear();
            for (int i = 0; i < myLoadedBullets.Count; ++i)
            {
                myBulletTypes.AddItem(myLoadedBullets[i].myType);
            }
        }

        protected override void LoadSettings()
        {
           
                myWeaponDataStats.GetCooldown().Text = myWeaponData.myCooldown.ToString();
                myWeaponDataStats.GetSpread().Text = myWeaponData.mySpread.ToString();
                myWeaponDataStats.GetPosition().SetPosition(myWeaponData.myPosition.myX.ToString()
                    , myWeaponData.myPosition.myY.ToString(), myWeaponData.myPosition.myZ.ToString());
                myWeaponDataStats.GetBulletPerShot().Text = myWeaponData.myNumberOfBulletsPerShot.ToString();
           
                myBulletDataStats.SetEntityType(myBulletData.myEntityType);
                myBulletDataStats.SetMaxAmount(myBulletData.myMaxAmount);
                myBulletDataStats.SetSpeed(myBulletData.mySpeed);
            
        }

        protected override void SaveSettings()
        {
            myShootingComponent.myWeaponType = (string)myWeaponTypes.GetDropDown().Text;

            
                    myWeaponData.myBulletType = (string)myBulletTypes.GetDropDown().Text;
                    myWeaponData.myCooldown = float.Parse(myWeaponDataStats.GetCooldown().Text);
                    myWeaponData.mySpread = int.Parse(myWeaponDataStats.GetSpread().Text);
                    myWeaponData.myNumberOfBulletsPerShot = int.Parse(myWeaponDataStats.GetBulletPerShot().Text);
                    myWeaponData.myPosition.myX = myWeaponDataStats.GetPosition().GetX();
                    myWeaponData.myPosition.myY = myWeaponDataStats.GetPosition().GetY();
                    myWeaponData.myPosition.myZ = myWeaponDataStats.GetPosition().GetZ();
            
                    myBulletData.myEntityType = myBulletDataStats.GetEntityType();
                    myBulletData.myMaxAmount = myBulletDataStats.GetMaxAmount();
                    myBulletData.mySpeed = myBulletDataStats.GetSpeed();
            

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetShootingComponent(myShootingComponent);
        }

        private bool CheckIfCanSaveWeapon()
        {
            if (myWeaponDataStats.GetBulletPerShot().Text == null 
                || myWeaponDataStats.GetBulletPerShot().Text == "") return false;
            if (myWeaponDataStats.GetCooldown().Text == null 
                || myWeaponDataStats.GetCooldown().Text == "") return false;
            if (myWeaponDataStats.GetSpread().Text == null 
                || myWeaponDataStats.GetSpread().Text == "") return false;
            if (myWeaponDataStats.GetPosition().GetXTextBox().Text == null 
                || myWeaponDataStats.GetPosition().GetXTextBox().Text == "") return false;
            if (myWeaponDataStats.GetPosition().GetYTextBox().Text == null 
                || myWeaponDataStats.GetPosition().GetYTextBox().Text == "") return false;
            if (myWeaponDataStats.GetPosition().GetZTextBox().Text == null 
                || myWeaponDataStats.GetPosition().GetZTextBox().Text == "") return false;
            if (myBulletTypes.GetDropDown().SelectedItem == null) return false;
            if ((string)myWeaponTypes.GetDropDown().SelectedItem != myWeaponData.myType) return false;

            return true;
        }

        private bool CheckIfCanSaveBullet()
        {
            if (myBulletDataStats.GetEntityTypeTextBox().Text == null 
                || myBulletDataStats.GetEntityTypeTextBox().Text == "") return false;
            if (myBulletDataStats.GetMaxAmountTextBox().Text == null 
                || myBulletDataStats.GetMaxAmountTextBox().Text == "") return false;
            if (myBulletDataStats.GetSpeedTextBox().Text == null 
                || myBulletDataStats.GetSpeedTextBox().Text == "") return false;
            if ((string)myBulletTypes.GetDropDown().SelectedItem != myBulletData.myType) return false;

            return true;
        }

        private void WeaponDropDown_SelectIndex_Changed(object sender, EventArgs e)
        {
            myWeaponData = myLoadedWeapons[myWeaponTypes.GetDropDown().SelectedIndex];
            LoadSettings();
        }

        private void BulletDropDown_SelectIndex_Changed(object sender, EventArgs e)
        {
            myBulletData = myLoadedBullets[myBulletTypes.GetDropDown().SelectedIndex];
            LoadSettings();
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            SaveSettings();
            string pathToData = myDataFolder.Replace("Data/", "");
            myWeaponWriter.SaveBulletFile(pathToData + myBulletData.myFilePath, myBulletData, myWeaponReader.GetBulletListXml());
            myWeaponWriter.SaveWeaponFile(pathToData + myWeaponData.myFilePath, myWeaponData, myWeaponReader.GetWeaponListXml());

            ReloadWeaponXML();
            LoadSettings();
        }
    }
}
