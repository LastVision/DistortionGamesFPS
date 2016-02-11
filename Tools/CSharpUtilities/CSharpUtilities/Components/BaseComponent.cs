using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    abstract public class BaseComponent
    {
        protected Point myLocation;
        protected Size mySize;
        protected String myPanelName;
        protected Components.IO.ComponentIO myIO;

        public BaseComponent(Point aLocation, Size aSize, Components.IO.ComponentIO aIO, string aPanelName)
        {
            myLocation = aLocation;
            mySize = aSize;
            myPanelName = aPanelName;
            myIO = aIO;

            InitializeComponents(myIO.myIOElementName);
        }

        abstract protected void InitializeComponents(string aText);

        abstract public void Show();
        abstract public void Hide();
        virtual public void Update()
        {

        }

        abstract public void BindToPanel(Panel aPanel);
        public String GetPanelName()
        {
            return myPanelName;
        }
    }
}
