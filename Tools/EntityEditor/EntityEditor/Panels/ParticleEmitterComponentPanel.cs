using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using CSharpUtilities;
using CSharpUtilities.Components;

namespace EntityEditor.Panels
{
    public class ParticleEmitterComponentPanel : BasePanel
    {
        private Entity.ParticleEmitterComponentData myParticleEmitterComponent;
        private PathBrowseComponent myEmitterXml;
        private OpenFileDialog myBrowseEmitterXML = new OpenFileDialog();
        public ParticleEmitterComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {

        }

        protected override void IntitalizeControls()
        {
            myPropertyTitle.Text += "Particle Emitter Component";

            myEmitterXml = new PathBrowseComponent(new Point(Location.X, Location.Y + 10), new Size(275, 13), "Emitter XML");
            myEmitterXml.GetButton().Click += new EventHandler(this.BroweseEmitterButton_Click);
            myEmitterXml.GetTextBox().TextChanged += new EventHandler(this.PanelDataChanged);
            myEmitterXml.BindToPanel(this);
            myEmitterXml.Show();
        }

        public void Load(Entity.ParticleEmitterComponentData aComponent)
        {
            myParticleEmitterComponent = aComponent;
            myHasLoadedComponent = true;
        }

        protected override void LoadSettings()
        {
            myEmitterXml.GetTextBox().Text = myParticleEmitterComponent.myEmitterXML;
        }

        protected override void SaveSettings()
        {
            myParticleEmitterComponent.myEmitterXML = myEmitterXml.GetTextBox().Text;

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetParticleEmitterComponent(myParticleEmitterComponent);
        }

        private void BroweseEmitterButton_Click(object sender, EventArgs e)
        {
            myBrowseEmitterXML.InitialDirectory = Directory.GetCurrentDirectory();
            myBrowseEmitterXML.DefaultExt = "*.xml";
            myBrowseEmitterXML.Filter = "XML file (*.xml)|*.xml";
            myBrowseEmitterXML.ShowDialog();

            myEmitterXml.GetTextBox().Text = CSharpUtilities.StringUtilities.ConvertPathToDataFolderPath(myBrowseEmitterXML.FileName);
        }
    }
}
