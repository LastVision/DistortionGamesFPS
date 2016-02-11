using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using CSharpUtilities;

namespace ParticleEditor
{
    public partial class ParticleEditor : Form
    {
        private CSharpUtilities.Components.DLLPreviewComponent myPreviewWindow;

        private string myCurrentParticleFile = "Data/Resource/Particle/P_default_health.xml";
        private string myCurrentFolderPath = Directory.GetCurrentDirectory();
        public ParticleEditor()
        {
            InitializeComponent();
            this.Text += " v.0.0";

            myPreviewWindow = new CSharpUtilities.Components.DLLPreviewComponent(new Point(0, 0), new Size(500, 600), "Preview", true);
            myPreviewWindow.BindToPanel(myParticleWindow);
            myPreviewWindow.Show();

            UpdateTimer.Start();
        }

        private void UpdateTimer_Tick(object sender, EventArgs e)
        {
            myPreviewWindow.Update();
        }

        private void FileNew_Click(object sender, EventArgs e)
        {

        }

        private void FileOpen_Click(object sender, EventArgs e)
        {
            myFileBrowserOpen.Filter = "Xml files(*.xml)|*.xml";
            myFileBrowserOpen.DefaultExt = "*.xml";
            myFileBrowserOpen.InitialDirectory = myCurrentFolderPath;
            myFileBrowserOpen.ShowDialog();

            myCurrentFolderPath = myFileBrowserOpen.FileName.Replace(myFileBrowserOpen.SafeFileName, "");
            myCurrentParticleFile = myFileBrowserOpen.FileName;

            CSharpUtilities.DLLImporter.NativeMethods.LoadParticle(myCurrentParticleFile);
        }

        private void FileSave_Click(object sender, EventArgs e)
        {

        }

        private void FileSaveAs_Click(object sender, EventArgs e)
        {

        }
    }
}
