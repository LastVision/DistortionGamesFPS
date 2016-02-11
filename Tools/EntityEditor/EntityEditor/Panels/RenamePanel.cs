using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using CSharpUtilities.Components;


namespace EntityEditor.Panels
{
    public class RenamePanel : BasePanel
    {
        private string myEntityName;

        private NumericTextComponent myNameText;

        public RenamePanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Entity Name";
        }

        public void Load(string aEntityName)
        {
            myEntityName = aEntityName;
            LoadSettings();
        }

        protected override void IntitalizeControls()
        {
            myNameText = new NumericTextComponent(new Point(Location.X, Location.Y + 10), new Size(245, 13), "Name", false);
            myNameText.GetTextBox().TextChanged += new EventHandler(this.NameTextBox_Changed);
            myNameText.BindToPanel(this);
            myNameText.Show();
        }

        protected override void LoadSettings()
        {
            myNameText.GetTextBox().Text = myEntityName;
        }

        protected override void SaveSettings()
        {
            myEntityName = myNameText.GetTextBox().Text;

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetEntityName(myEntityName);
        }

        private void NameTextBox_Changed(object sender, EventArgs e)
        {
            SaveSettings();
        }
    }
}
