using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    public class DLLPreviewComponent : BaseComponent
    {
        private Label myLabel = new Label();
        private Panel myPreviewWindow = new Panel();

        private bool myUseText;

        public DLLPreviewComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName, bool aUseText = false)
            : base(aLocation, aSize, aIO, aPanelName)
        {
            myUseText = aUseText;
        }

        protected override void InitializeComponents(string aText)
        {
            myLabel.Text = "Preview";
            myLabel.Location = new Point(myLocation.X, myLocation.Y - 13);
            myLabel.Size = new Size(100, 13);

            if (myUseText == true)
            {
                myLabel.Show();   
            }
            else
            {
                myLabel.Hide();
            }

            myPreviewWindow.Location = new Point(myLocation.X, myLocation.Y);
            myPreviewWindow.Size = new Size(mySize.Width, mySize.Height - 13);
            myPreviewWindow.Show();

            IntPtr previewWindowHandler = myPreviewWindow.Handle;

            myPreviewWindow.Invalidate();

            DLLImporter.NativeMethods.SetupWindow(mySize.Width, mySize.Height);
            DLLImporter.NativeMethods.StartEngine(previewWindowHandler);
            DLLImporter.NativeMethods.Render();
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);
            aPanel.Controls.Add(myPreviewWindow);
        }

        public override void Hide()
        {
            myPreviewWindow.Hide();
            myLabel.Hide();
        }

        public override void Show()
        {
            myPreviewWindow.Show();
            if (myUseText == true)
            {
                myLabel.Show();
            }
        }

        public override void Update()
        {
            myPreviewWindow.Invalidate();

            DLLImporter.NativeMethods.Update();
            DLLImporter.NativeMethods.UpdateFilewatcher();
            DLLImporter.NativeMethods.Render();
        }
    }
}
