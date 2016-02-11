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
    public class GCP_ModelPanel
    {
        private PathBrowseComponent myModelPathComponent;
        private PathBrowseComponent myEffectPathComponent;

        private Point myLocation;

        public GCP_ModelPanel(Point aLocation)
        {
            myLocation = aLocation;

            myModelPathComponent = new PathBrowseComponent(new Point(myLocation.X, myLocation.Y + 10), new Size(300, 10), "Model");
            myEffectPathComponent = new PathBrowseComponent(new Point(myLocation.X, myLocation.Y + 35), new Size(300, 10), "Effect");
        }

        public void Show()
        {
            myModelPathComponent.Show();
            myEffectPathComponent.Show();
        }

        public PathBrowseComponent GetModelPath()
        {
            return myModelPathComponent;
        }

        public PathBrowseComponent GetEffectPath()
        {
            return myEffectPathComponent;
        }

        public void BindToPanel(Panel aPanel)
        {
            myModelPathComponent.BindToPanel(aPanel);
            myEffectPathComponent.BindToPanel(aPanel);
        }
    }

    public class GraphicsComponentPanel : BasePanel
    {
        private GCP_ModelPanel myModelPanel;

        private Vector3Component myScale;

        private OpenFileDialog myBrowseDialog = new OpenFileDialog();
        private string myGraphicsFolder;

        private Entity.GraphicsComponentData myGraphicsComponentData;

        public GraphicsComponentPanel(Point aLocation, Size aSize, Form aParent) : base(aLocation, aSize, aParent)
        {
            
        }

        public void Load(Entity.GraphicsComponentData aGraphicsComponentData)
        {
            myGraphicsComponentData = aGraphicsComponentData;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        protected override void IntitalizeControls()
        {
            myPropertyTitle.Text += "Graphcis Component";;

            myModelPanel = new GCP_ModelPanel(new Point(this.Location.X, this.Location.Y));

            myModelPanel.BindToPanel(this);

            myModelPanel.GetModelPath().GetButton().Click += new System.EventHandler(this.ModelBrosweClick);
            myModelPanel.GetEffectPath().GetButton().Click += new System.EventHandler(this.EffectBrosweClick);

            myModelPanel.Show();

            myScale = new Vector3Component(new Point(Location.X, Location.Y + 60), new Size(275, 40), "Scale");
            myScale.AddTextChangeEvent(this.PanelDataChanged);
            myScale.BindToPanel(this);
            myScale.Show();
        }

        protected override void LoadSettings()
        {
            myModelPanel.GetModelPath().GetTextBox().Text = myGraphicsComponentData.myModelPath;
            myModelPanel.GetEffectPath().GetTextBox().Text = myGraphicsComponentData.myEffectPath;

            myScale.SetPosition(myGraphicsComponentData.myScale);
        }

        protected override void SaveSettings()
        {
            myGraphicsComponentData.myModelPath = myModelPanel.GetModelPath().GetTextBox().Text;
            myGraphicsComponentData.myEffectPath = myModelPanel.GetEffectPath().GetTextBox().Text;

            myGraphicsComponentData.myScale.myX = myScale.GetX();
            myGraphicsComponentData.myScale.myY = myScale.GetY();
            myGraphicsComponentData.myScale.myZ = myScale.GetZ();

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetGraphicsComponent(myGraphicsComponentData);
        }

        private void ModelBrosweClick(object sender, EventArgs e)
        {
            myBrowseDialog.Filter = "Model files(*.fbx)|*.fbx";

            if(myGraphicsFolder != "")
            {
                myBrowseDialog.InitialDirectory = myGraphicsFolder;
            }
            myBrowseDialog.ShowDialog();

            if(myBrowseDialog.FileName != null)
            {
                string filePath = StringUtilities.ConvertPathToOwnFolder(myBrowseDialog.FileName, 
                    myBrowseDialog.SafeFileName, "Data/Resource/Model");

                myModelPanel.GetModelPath().GetTextBox().Text = filePath;
            }

            myGraphicsFolder = myBrowseDialog.FileName.Replace(myBrowseDialog.SafeFileName, "");
            Properties.Settings.Default.DefualtDataFolder = myGraphicsFolder;
            Properties.Settings.Default.Save();
            myBrowseDialog.Filter = "";

            SaveSettings();
        }

        private void EffectBrosweClick(object sender, EventArgs e)
        {
            myBrowseDialog.Filter = "Shader files(*.fx)|*.fx";

            if (myGraphicsFolder != "")
            {
                myBrowseDialog.InitialDirectory = myGraphicsFolder;
            }
            myBrowseDialog.ShowDialog();

            if (myBrowseDialog.FileName != null)
            {
                string filePath = StringUtilities.ConvertPathToOwnFolder(myBrowseDialog.FileName,
                    myBrowseDialog.SafeFileName, "Data/Resource");

                myModelPanel.GetEffectPath().GetTextBox().Text = filePath;
            }

            myGraphicsFolder = myBrowseDialog.FileName.Replace(myBrowseDialog.SafeFileName, "");
            Properties.Settings.Default.DefualtDataFolder = myGraphicsFolder;
            Properties.Settings.Default.Save();
            myBrowseDialog.Filter = "";

            SaveSettings();
        }
    }
}
