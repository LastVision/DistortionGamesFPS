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
    public class BulletComponentPanel : BasePanel
    {
        private Entity.BulletComponentData myBulletComponent;

        private NumericTextComponent myLifeTime;
        private NumericTextComponent myDamage;
        public BulletComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Bullet Component";
        }

        protected override void IntitalizeControls()
        {
            myLifeTime = new NumericTextComponent(new Point(Location.X, Location.Y + 10), new Size(175, 13), "Life Time");
            myLifeTime.AddTextChangeEvent(this.PanelDataChanged);
            myLifeTime.BindToPanel(this);
            myLifeTime.Show();

            myDamage = new NumericTextComponent(new Point(Location.X, Location.Y + 30), new Size(175, 13), "Damage");
            myDamage.AddTextChangeEvent(this.PanelDataChanged);
            myDamage.BindToPanel(this);
            myDamage.Show();
        }

        public void Load(Entity.BulletComponentData aBulletComponent)
        {
            myBulletComponent = aBulletComponent;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        protected override void LoadSettings()
        {
            myHasLoadedComponent = false;

            myLifeTime.GetTextBox().Text = myBulletComponent.myLifeTime.ToString();
            myDamage.GetTextBox().Text = myBulletComponent.myDamage.ToString();

            myHasLoadedComponent = true;
        }

        protected override void SaveSettings()
        {
            myBulletComponent.myLifeTime = StringUtilities.ToFloat(myLifeTime.GetTextBox().Text);
            myBulletComponent.myDamage = StringUtilities.ToFloat(myDamage.GetTextBox().Text);

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetBulletComponent(myBulletComponent);
        }
    }
}
