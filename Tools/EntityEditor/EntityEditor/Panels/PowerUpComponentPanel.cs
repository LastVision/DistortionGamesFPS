using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Windows.Forms;
using CSharpUtilities;
using CSharpUtilities.Components;

namespace EntityEditor.Panels
{
    public class PowerUpComponentPanel : BasePanel
    {
        private Entity.PowerUpComponentData myPowerUpComponent;

        private DropDownComponent myType;

        private NumericTextComponent myValue;
        private NumericTextComponent myTime;

        private NumericTextComponent myWeaponUpgrade;
        private NumericTextComponent myWeaponID;

        public PowerUpComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "PowerUp Component";
        }

        protected override void IntitalizeControls()
        {
            myType = new DropDownComponent(new Point(Location.X, Location.Y + 10), new Size(150, 13), "Type");
            myType.AddSelectedIndexChangeEvent(this.PanelDataChanged);
            myType.AddSelectedIndexChangeEvent(this.TypeChanged);
            myType.BindToPanel(this);
            myType.Show();

            myValue = new NumericTextComponent(new Point(Location.X, Location.Y + 40), new Size(100, 13), "Value");
            myValue.AddTextChangeEvent(this.PanelDataChanged);
            myValue.BindToPanel(this);
            myValue.Show();

            myTime = new NumericTextComponent(new Point(Location.X + 100, Location.Y + 40), new Size(100, 13), "Time");
            myTime.AddTextChangeEvent(this.PanelDataChanged);
            myTime.BindToPanel(this);
            myTime.Show();

            myWeaponUpgrade = new NumericTextComponent(new Point(Location.X, Location.Y + 40), new Size(200, 13), "Upgrade", false);
            myWeaponUpgrade.AddTextChangeEvent(this.PanelDataChanged);
            myWeaponUpgrade.BindToPanel(this);
            myWeaponUpgrade.Hide();

            myWeaponID = new NumericTextComponent(new Point(Location.X + 200, Location.Y + 40), new Size(70, 13), "ID");
            myWeaponID.AddTextChangeEvent(this.PanelDataChanged);
            myWeaponID.BindToPanel(this);
            myWeaponID.Hide();

            myType.AddItem("WeaponUpgrade");
            myType.AddItem("firerate");
            myType.AddItem("healthkit");
            myType.AddItem("homing");
            myType.AddItem("shield");

        }

        public void Load(Entity.PowerUpComponentData aPowerUpComponent)
        {
            myPowerUpComponent = aPowerUpComponent;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        protected override void LoadSettings()
        {
            myHasLoadedComponent = false;

            myValue.GetTextBox().Text = myPowerUpComponent.myValue.ToString();
            myTime.GetTextBox().Text = myPowerUpComponent.myTime.ToString();

            myWeaponUpgrade.GetTextBox().Text = myPowerUpComponent.myUpgradedWeapon;
            myWeaponID.GetTextBox().Text = myPowerUpComponent.myWeaponID.ToString();

            myType.GetDropDown().SelectedItem = myPowerUpComponent.myType;

            myHasLoadedComponent = true;
        }

        protected override void SaveSettings()
        {
            myPowerUpComponent.myType = (string)myType.GetDropDown().SelectedItem;

            myPowerUpComponent.myValue = StringUtilities.ToFloat(myValue.GetTextBox().Text);
            myPowerUpComponent.myTime = StringUtilities.ToFloat(myTime.GetTextBox().Text);

            myPowerUpComponent.myUpgradedWeapon = myWeaponUpgrade.GetTextBox().Text;
            myPowerUpComponent.myWeaponID = StringUtilities.ToInt(myWeaponID.GetTextBox().Text);

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetPowerUpComponent(myPowerUpComponent);
        }

        private void TypeChanged(object sender, EventArgs e)
        {
            if ((string)myType.GetDropDown().SelectedItem == "WeaponUpgrade")
            {
                myValue.Hide();
                myTime.Hide();
                myWeaponUpgrade.Show();
                myWeaponID.Show();
            }
            else
            {
                myValue.Show();
                myTime.Show();
                myWeaponUpgrade.Hide();
                myWeaponID.Hide();
            }
        }
    }
}
