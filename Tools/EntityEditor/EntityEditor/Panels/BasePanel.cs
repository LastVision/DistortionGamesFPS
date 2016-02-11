using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Windows.Forms;

namespace EntityEditor.Panels
{
    abstract public class BasePanel : Panel
    {
        protected Label myPropertyTitle = new Label();
        protected Form myOwnerForm;
        protected bool myHasLoadedComponent = false;
        public BasePanel(Point aLocation, Size aSize, Form aParent)
        {
            myOwnerForm = aParent;

            Location = aLocation;
            Size = aSize;

            myPropertyTitle.Text = "Property - ";
            myPropertyTitle.Location = new Point(0, 0);
            myPropertyTitle.Size = new Size(aSize.Width, 12);

            Controls.Add(myPropertyTitle);

            myPropertyTitle.Show();

            IntitalizeControls();
        }

        abstract protected void IntitalizeControls();
        abstract protected void LoadSettings();
        abstract protected void SaveSettings();
        protected void PanelDataChanged(object sender, EventArgs e)
        {
            if (myHasLoadedComponent == true)
            {
                SaveSettings();
                LoadSettings();
            }
        }
    }
}
