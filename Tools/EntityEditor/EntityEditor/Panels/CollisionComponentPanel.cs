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
    public class CollisionComponentPanel : BasePanel
    {
        private Entity.CollisionComponentData myCollisionComponent;

        private CheckBox mySphereCollisionActive = new CheckBox();
        private NumericTextComponent mySphereRadius;

        public CollisionComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Collision Component";
        }

        protected override void IntitalizeControls()
        {
            mySphereCollisionActive.Text = "Sphere Active";
            mySphereCollisionActive.RightToLeft = RightToLeft.Yes;
            mySphereCollisionActive.Location = new Point(Location.X - 58, Location.Y + 10);
            mySphereCollisionActive.Size = new Size(150, 20);

            mySphereCollisionActive.CheckStateChanged += new EventHandler(this.PanelDataChanged);

            this.Controls.Add(mySphereCollisionActive);
            mySphereCollisionActive.Show();

            mySphereRadius = new NumericTextComponent(new Point(Location.X, Location.Y + 40), new Size(245, 13), "Sphere Radius");
            mySphereRadius.AddTextChangeEvent(this.PanelDataChanged);
            mySphereRadius.BindToPanel(this);
            mySphereRadius.Show();
        }

        public void Load(Entity.CollisionComponentData aCollisionComponent)
        {
            myHasLoadedComponent = false;
            myCollisionComponent = aCollisionComponent;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        protected override void LoadSettings()
        {
            if(myCollisionComponent.myHasSphere == true)
            {
                mySphereCollisionActive.CheckState = CheckState.Checked;
                mySphereRadius.GetTextBox().Enabled = true;
                mySphereRadius.GetTextBox().Text = myCollisionComponent.myRadius.ToString();
            }
            else 
            {
                mySphereCollisionActive.CheckState = CheckState.Unchecked;
                mySphereRadius.GetTextBox().Text = "";
                mySphereRadius.GetTextBox().Enabled = false;
            }
        }

        protected override void SaveSettings()
        {
            if(mySphereCollisionActive.CheckState == CheckState.Checked)
            {
                myCollisionComponent.myHasSphere = true;
                if (mySphereRadius.GetTextBox().Text != "")
                {
                    myCollisionComponent.myRadius = StringUtilities.ToFloat(mySphereRadius.GetTextBox().Text);
                }
            }
            else
            {
                myCollisionComponent.myHasSphere = false;
                myCollisionComponent.myRadius = 0;
            }
            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetCollisionComponent(myCollisionComponent);
        }
    }
}
