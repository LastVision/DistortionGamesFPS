using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using CSharpUtilities.Components;

namespace ToolBuilder
{
    public partial class Tool_Window : Form
    {
        private String myCurrentToolFile = "Tool/TBE_viewer_particle.xml";
        private ToolBuilder.ToolReader myReader = null;
        private List<BaseComponent> myComponents = null;

        public Tool_Window()
        {
            InitializeComponent();
            myReader = new ToolBuilder.ToolReader(myCurrentToolFile, ref Tool_Window_Main, ref Tool_Window_Property);
            myComponents = myReader.GetComponents();
            myTimer.Start();
        }

        private void OnTick(object sender, EventArgs e)
        {
            for(int i = 0; i < myComponents.Count; ++i)
            {
                myComponents[i].Update();
            }
        }
    }
}
