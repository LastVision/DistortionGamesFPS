using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using CSharpUtilities;
using CSharpUtilities.Components;

namespace EntityEditor.Panels
{
    public class HealthComponentPanel : BasePanel
    {
        private Entity.HealthComponentData myHealthComponent;

        private NumericTextComponent myHealth;

        public HealthComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Health Component";
        }

        protected override void IntitalizeControls()
        {
            myHealth = new NumericTextComponent(new Point(Location.X, Location.Y + 10), new Size(175, 13), "Health");
            myHealth.AddTextChangeEvent(this.PanelDataChanged);
            myHealth.BindToPanel(this);
            myHealth.Show();
        }

        public void Load(Entity.HealthComponentData aHealthComponent)
        {
            myHasLoadedComponent = false;
            myHealthComponent = aHealthComponent;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        protected override void LoadSettings()
        {
            myHealth.GetTextBox().Text = myHealthComponent.myHealth.ToString();
        }

        protected override void SaveSettings()
        {
            myHealthComponent.myHealth = StringUtilities.ToInt(myHealth.GetTextBox().Text);

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetHealthComponent(myHealthComponent);
        }
    }
}
