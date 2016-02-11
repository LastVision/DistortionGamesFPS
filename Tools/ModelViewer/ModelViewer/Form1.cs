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

namespace ModelViewer
{
    public partial class ModelViewerWindow : Form
    {
        private List<string> myModelFiles = null;
        private List<string> myShaderFiles = null;
        private List<string> myCubeMapFiles = null;

        private string myCurrentShaderFilePath = Properties.Settings.Default.DefaultEffectFilePath;
        private string myCurrentModelFilePath = Properties.Settings.Default.DefaultModelFilePath;

        private string myDataFolderPath = Properties.Settings.Default.DefaultDataFolderPath;

        private Point myPreviousMousePosition;

        private Button myChooseDataFolderButton = new Button();
        private CheckBox myEnableAutoRotation = new CheckBox();

        private CSharpUtilities.Components.DropDownComponent myModelList;
        private CSharpUtilities.Components.DropDownComponent myShaderList;
        private CSharpUtilities.Components.DropDownComponent myCubeMapList;

        private CSharpUtilities.Components.DLLPreviewComponent myPreviewWindow;

        private CSharpUtilities.Components.Vector3SliderComponent myDirectionalLightRotation;
        private CSharpUtilities.Components.Vector3SliderComponent myObjectManualRotation;
        private CSharpUtilities.Components.Vector3SliderComponent myObjectAutoRotation;
        private CSharpUtilities.Components.Vector3SliderComponent myCameraSettingsSliders;

        private CSharpUtilities.Components.SliderComponent myMouseSensitivitySlider;

        public ModelViewerWindow()
        {
            InitializeComponent();

            if (myDataFolderPath == "")
            {
                Properties.Settings.Default.DefaultDataFolderPath = Directory.GetCurrentDirectory();
                myDataFolderPath = Properties.Settings.Default.DefaultDataFolderPath;
                Properties.Settings.Default.Save();
            }

            DataFolderBrowser.SelectedPath = myDataFolderPath;

            myPreviousMousePosition = MousePosition;

            myChooseDataFolderButton.Text = "Open";
            myChooseDataFolderButton.Location = new Point(Location.X + 5, Location.Y+10);
            myChooseDataFolderButton.Size = new Size(50, 20);
            myChooseDataFolderButton.Show();
            myChooseDataFolderButton.Click += new EventHandler(this.Btn_OpenDataFolder_Click);
            Menu_Panel.Controls.Add(myChooseDataFolderButton);

            myModelList = new CSharpUtilities.Components.DropDownComponent(new Point(Location.X + 60, Location.Y+10), new Size(250, 13), "Model: ");
            myModelList.AddSelectedIndexChangeEvent(this.ModelList_SelectedIndexChanged);
            myModelList.BindToPanel(Menu_Panel);
            myModelList.Show();

            myShaderList = new CSharpUtilities.Components.DropDownComponent(new Point(Location.X + 310, Location.Y+10), new Size(250, 13), "Shader: ");
            myShaderList.AddSelectedIndexChangeEvent(this.ShaderList_SelectedIndexChanged);
            myShaderList.BindToPanel(Menu_Panel);
            myShaderList.Show();

            myCubeMapList = new CSharpUtilities.Components.DropDownComponent(new Point(Location.X + 560, Location.Y + 10), new Size(250, 13), "Cube Map: ");
            myCubeMapList.AddSelectedIndexChangeEvent(this.CubeMapList_SelectedIndexChanged);
            myCubeMapList.BindToPanel(Menu_Panel);
            myCubeMapList.Show();

            myPreviewWindow = new CSharpUtilities.Components.DLLPreviewComponent(new Point(ModelViewer.Location.X, ModelViewer.Location.Y - 20), ModelViewer.Size, "Preview", true);
            myPreviewWindow.BindToPanel(ModelViewer);
            myPreviewWindow.Show();

            myDirectionalLightRotation = new CSharpUtilities.Components.Vector3SliderComponent(new Point(0, 0), new Size(200, 50), "Directional Light Rotation", -360, 360, 180, true);
            myDirectionalLightRotation.AddSelectedValueChangedEvent(this.DirectionalLight_Scroll);
            myDirectionalLightRotation.BindToPanel(ModelViewerMenu);
            myDirectionalLightRotation.Show();

            myDirectionalLightRotation.SetXValue(CSharpUtilities.DLLImporter.NativeMethods.GetDirectionaLightXRotation());
            myDirectionalLightRotation.SetYValue(CSharpUtilities.DLLImporter.NativeMethods.GetDirectionaLightYRotation());
            myDirectionalLightRotation.SetZValue(CSharpUtilities.DLLImporter.NativeMethods.GetDirectionaLightZRotation());

            myObjectManualRotation = new CSharpUtilities.Components.Vector3SliderComponent(new Point(0, 60), new Size(200, 50), "Object Manual Rotation", -180, 180, 0, false);
            myObjectManualRotation.AddSelectedXValueChangedEvent(this.ManualObjectXRotation_Scroll);
            myObjectManualRotation.AddSelectedYValueChangedEvent(this.ManualObjectYRotation_Scroll);
            myObjectManualRotation.AddSelectedZValueChangedEvent(this.ManualObjectZRotation_Scroll);
            myObjectManualRotation.BindToPanel(ModelViewerMenu);
            myObjectManualRotation.Hide();

            myObjectAutoRotation = new CSharpUtilities.Components.Vector3SliderComponent(new Point(0, 60), new Size(200, 50), "Object Auto Rotation", -360, 360, 0, true);
            myObjectAutoRotation.AddSelectedValueChangedEvent(this.AutoRotation_Scroll);
            myObjectAutoRotation.BindToPanel(ModelViewerMenu);
            myObjectAutoRotation.Show();

            myEnableAutoRotation.Text = "Activate Auto Rotation";
            myEnableAutoRotation.CheckedChanged += new EventHandler(this.EnableAutoRotation_CheckStateChanged);
            myEnableAutoRotation.Location = new Point(10, 120);
            myEnableAutoRotation.Size = new Size(200, 20);
            myEnableAutoRotation.Checked = true;
            myEnableAutoRotation.Show();
            ModelViewerMenu.Controls.Add(myEnableAutoRotation);

            myMouseSensitivitySlider = new CSharpUtilities.Components.SliderComponent(new Point(0, 150), new Size(200, 15), "Mouse Sens", 0, 1000, 1, true, 0.0f, 100.0f);
            myMouseSensitivitySlider.AddSelectedValueChangedEvent(this.MouseSensitivity_Changed);
            myMouseSensitivitySlider.SetValue(Properties.Settings.Default.DefaultSettingMouseSensitivity);
            myMouseSensitivitySlider.BindToPanel(ModelViewerMenu);
            myMouseSensitivitySlider.Show();

            myCameraSettingsSliders = new CSharpUtilities.Components.Vector3SliderComponent(new Point(0, 175), new Size(200, 50), "Camera Settings", -1000, 1000, 0, true, "Zoom: ", "Movement: ", "Rotation: ", -10.0f, 10.0f);
            myCameraSettingsSliders.SetXValue(Properties.Settings.Default.DefaultSettingCameraZoom);
            myCameraSettingsSliders.SetYValue(Properties.Settings.Default.DefaultSettingCameraMovement);
            myCameraSettingsSliders.AddSelectedValueChangedEvent(this.CameraSettings_Changed);
            myCameraSettingsSliders.SetZValue(Properties.Settings.Default.DefaultSettingCameraRotation);
            myCameraSettingsSliders.BindToPanel(ModelViewerMenu);
            myCameraSettingsSliders.Show();

            FillModelList();
            FillShaderList();
            FillCubeMapList();

            UpdateTimer.Start();
        }


        private void Btn_OpenDataFolder_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(myDataFolderPath) == false)
            {
                myDataFolderPath = Directory.GetCurrentDirectory();
            }
            DataFolderBrowser.SelectedPath = myDataFolderPath;
            DataFolderBrowser.ShowDialog();
            myDataFolderPath = DataFolderBrowser.SelectedPath;

            if (myDataFolderPath != "")
            {
                FillModelList();
                FillShaderList();
                FillCubeMapList();
            }

            Properties.Settings.Default.DefaultDataFolderPath = myDataFolderPath;
            Properties.Settings.Default.Save();
        }

        void FillModelList()
        {
            myModelList.GetDropDown().Items.Clear();
            myModelFiles = Directory.GetFiles(myDataFolderPath, "*.fbx", SearchOption.AllDirectories).ToList();
            for (int i = 0; i < myModelFiles.Count; ++i)
            {
                string file = StringUtilities.ConvertPathToRelativePath(myModelFiles[i], "Model\\");
                myModelList.AddItem(file);
            }
        }

        private void FillShaderList()
        {
            myShaderList.GetDropDown().Items.Clear();
            myShaderFiles = Directory.GetFiles(myDataFolderPath, "*.fx", SearchOption.AllDirectories).ToList();
            for (int i = 0; i < myShaderFiles.Count; ++i)
            {
                string file = StringUtilities.ConvertPathToRelativePath(myShaderFiles[i], "Shader\\");
                if (file.StartsWith("S_effect") == true && VerifyShader(file) == true)
                {
                    myShaderList.AddItem(file);
                }
            }
        }

        private void FillCubeMapList()
        {
            myCubeMapList.GetDropDown().Items.Clear();
            myCubeMapFiles = Directory.GetFiles(myDataFolderPath, "*.dds", SearchOption.AllDirectories).ToList();
            for (int i = 0; i < myCubeMapFiles.Count; ++i)
            {
                string file = StringUtilities.ConvertPathToRelativePath(myCubeMapFiles[i], "CubeMap\\");
                if (file.StartsWith("T_cubemap") == true)
                {
                    myCubeMapList.AddItem(file);
                }
            }
        }

        private bool VerifyShader(string aFileName)
        {
            if (aFileName.EndsWith("pbl.fx")
                || aFileName.EndsWith("basic.fx"))
            {
                return true;
            }
            return false;
        }

        private void ModelList_SelectedIndexChanged(object sender, EventArgs e)
        {
            string selectedItem = (string)myModelList.GetDropDown().SelectedItem;
            selectedItem = selectedItem.Replace("/", "\\");
            for (int i = 0; i < myModelFiles.Count; ++i)
            {
                if (myModelFiles[i].EndsWith(selectedItem) == true)
                {
                    myCurrentModelFilePath = myModelFiles[i];
                    break;
                }
            }
            if (myCurrentShaderFilePath != "" && myCurrentModelFilePath != "")
            {
                Btn_LoadModel_Click(sender, e);
            }
        }

        private void ShaderList_SelectedIndexChanged(object sender, EventArgs e)
        {
            string selectedItem = (string)myShaderList.GetDropDown().SelectedItem;
            selectedItem = selectedItem.Replace("/", "\\");
            for (int i = 0; i < myShaderFiles.Count; ++i)
            {
                if (myShaderFiles[i].EndsWith(selectedItem) == true)
                {
                    myCurrentShaderFilePath = myShaderFiles[i];
                    break;
                }
            }
            if (myCurrentShaderFilePath != "" && myCurrentModelFilePath != "")
            {
                Btn_LoadModel_Click(sender, e);
            }
        }

        private void CubeMapList_SelectedIndexChanged(object sender, EventArgs e)
        {
            string selectedItem = (string)myCubeMapList.GetDropDown().SelectedItem;
            selectedItem = selectedItem.Replace("/", "\\");
            for (int i = 0; i < myShaderFiles.Count; ++i)
            {
                if (myCubeMapFiles[i].EndsWith(selectedItem) == true)
                {
                    CSharpUtilities.DLLImporter.NativeMethods.SetCubeMap(myCubeMapFiles[i]);
                    break;
                }
            }
        }


        private void UpdateTimer_Tick(object sender, EventArgs e)
        {
            myPreviewWindow.Update();
        }

        private void Btn_LoadModel_Click(object sender, EventArgs e)
        {
            if (myCurrentModelFilePath == "")
            {
                MessageBox.Show("Error: No model file is selected");
                return;
            }
            if (myCurrentShaderFilePath == "")
            {
                MessageBox.Show("Error: No effect file is selected");
                return;
            }
            if (myCurrentModelFilePath != "" && myCurrentShaderFilePath != "")
            {
                if (Path.GetExtension(myCurrentModelFilePath) == ".fbx")
                {
                    CSharpUtilities.DLLImporter.NativeMethods.LoadModel(myCurrentModelFilePath, myCurrentShaderFilePath);
                }
                else
                {
                    MessageBox.Show("Error: " + Path.GetExtension(myCurrentModelFilePath) + " is not compatible. \nTry using a .fbx file instead.");
                }
            }
        }

        private void Btn_BackgroundColor_Click(object sender, EventArgs e)
        {
            BackgroundColorDialog.ShowDialog();
            Btn_BackgroundColor.BackColor = BackgroundColorDialog.Color;

            float redChannel =   (BackgroundColorDialog.Color.R)/ 255.0f;
            float greenChannel = (BackgroundColorDialog.Color.G)/ 255.0f;
            float blueChannel =  (BackgroundColorDialog.Color.B)/ 255.0f;
            float alphaChannel = (BackgroundColorDialog.Color.A)/ 255.0f;

            CSharpUtilities.DLLImporter.NativeMethods.SetClearColor(redChannel, greenChannel, blueChannel, alphaChannel);
        }

        private void DirectionalLight_Scroll(object sender, EventArgs e)
        {
            CSharpUtilities.DLLImporter.NativeMethods.DirectionaLightRotateX(myDirectionalLightRotation.GetXValue());
            CSharpUtilities.DLLImporter.NativeMethods.DirectionaLightRotateY(myDirectionalLightRotation.GetZValue());
            CSharpUtilities.DLLImporter.NativeMethods.DirectionaLightRotateZ(myDirectionalLightRotation.GetYValue());
        }

        private void ManualObjectXRotation_Scroll(object sender, EventArgs e)
        {
            if (myEnableAutoRotation.Checked == true) return;

            CSharpUtilities.DLLImporter.NativeMethods.SetRotateObjectAtX(myObjectManualRotation.GetXValue());
        }

        private void ManualObjectYRotation_Scroll(object sender, EventArgs e)
        {
            if (myEnableAutoRotation.Checked == true) return;

            CSharpUtilities.DLLImporter.NativeMethods.SetRotateObjectAtY(myObjectManualRotation.GetYValue());
        }

        private void ManualObjectZRotation_Scroll(object sender, EventArgs e)
        {
            if (myEnableAutoRotation.Checked == true) return;

            CSharpUtilities.DLLImporter.NativeMethods.SetRotateObjectAtZ(myObjectManualRotation.GetZValue());
        }

        private void AutoRotation_Scroll(object sender, EventArgs e)
        {
            if (myEnableAutoRotation.Checked == false) return;
            CSharpUtilities.DLLImporter.NativeMethods.RotateObjectAtX(myObjectAutoRotation.GetXValue());
            CSharpUtilities.DLLImporter.NativeMethods.RotateObjectAtY(myObjectAutoRotation.GetYValue());
            CSharpUtilities.DLLImporter.NativeMethods.RotateObjectAtZ(myObjectAutoRotation.GetZValue());
        }

        private void EnableAutoRotation_CheckStateChanged(object sender, EventArgs e)
        {
            if (myEnableAutoRotation.Checked == true)
            {
                myObjectAutoRotation.Show();
                myObjectManualRotation.Hide();
            }
            else
            {
                myObjectAutoRotation.Hide();
                myObjectManualRotation.Show();
            }
            AutoRotation_Scroll(sender, e);
            ManualObjectXRotation_Scroll(sender, e);
            ManualObjectYRotation_Scroll(sender, e);
            ManualObjectZRotation_Scroll(sender, e);
        }

        private void MouseSensitivity_Changed(object sender, EventArgs e)
        {
            CSharpUtilities.DLLImporter.NativeMethods.SetMouseSensitivty(myMouseSensitivitySlider.GetValue());
            Properties.Settings.Default.DefaultSettingMouseSensitivity = myMouseSensitivitySlider.GetValue();
            Properties.Settings.Default.Save();
        }

        private void CameraSettings_Changed(object sender, EventArgs e)
        {
            CSharpUtilities.DLLImporter.NativeMethods.SetCameraZoomSpeed(myCameraSettingsSliders.GetXValue());
            CSharpUtilities.DLLImporter.NativeMethods.SetCameraMovementSpeed(myCameraSettingsSliders.GetYValue());
            CSharpUtilities.DLLImporter.NativeMethods.SetCameraRotationSpeed(myCameraSettingsSliders.GetZValue());

            float xValue = myCameraSettingsSliders.GetXValue();

            Properties.Settings.Default.DefaultSettingCameraZoom = myCameraSettingsSliders.GetXValue();
            Properties.Settings.Default.DefaultSettingCameraMovement = myCameraSettingsSliders.GetYValue();
            Properties.Settings.Default.DefaultSettingCameraRotation = myCameraSettingsSliders.GetZValue();
            Properties.Settings.Default.Save();
        }
    }
}
