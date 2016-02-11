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
    public class PhysicsComponentPanel : BasePanel
    {
        private Entity.PhysicsComponentData myPhysicsComponent;

        private NumericTextComponent myWeight;

        public PhysicsComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Physics Component";
        }

        protected override void IntitalizeControls()
        {
            myWeight = new NumericTextComponent(new Point(Location.X, Location.Y + 10), new Size(175, 13), "Weight");
            myWeight.AddTextChangeEvent(this.PanelDataChanged);
            myWeight.BindToPanel(this);
            myWeight.Show();
        }

        public void Load(Entity.PhysicsComponentData aPhysicsComponent)
        {
            myPhysicsComponent = aPhysicsComponent;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        protected override void LoadSettings()
        {
            myHasLoadedComponent = false;

            myWeight.GetTextBox().Text = myPhysicsComponent.myWeight.ToString();

            myHasLoadedComponent = true;
        }

        protected override void SaveSettings()
        {
            myPhysicsComponent.myWeight = StringUtilities.ToFloat(myWeight.GetTextBox().Text);

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetPhysicsComponent(myPhysicsComponent);
        }
    }
}
