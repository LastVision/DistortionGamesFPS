using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    public class PathBrowseComponent : BaseComponent
    {
        private Label myText = new Label();
        private TextBox myTextBox = new TextBox();
        private Button myButton = new Button();

        public PathBrowseComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName)
            : base(aLocation, aSize, aIO, aPanelName)
        {

        }

        protected override void InitializeComponents(string aText)
        {
            int sizeXPerStep = mySize.Width / 6;

            myText.Text = aText;
            myText.Location = new Point(myLocation.X, myLocation.Y + 3);
            myText.Size = new Size(sizeXPerStep, mySize.Height + 2);

            myTextBox.Location = new Point(myLocation.X + sizeXPerStep, myLocation.Y);
            myTextBox.Size = new Size(sizeXPerStep * 4, mySize.Height);
            myTextBox.ReadOnly = true;
            myTextBox.BackColor = Color.White;

            myButton.Text = "Browse";
            myButton.Location = new Point(myLocation.X + (sizeXPerStep * 5), myLocation.Y);
            myButton.Size = new Size(sizeXPerStep, mySize.Height + 10);
        }

        public override void Show()
        {
            myText.Show();
            myTextBox.Show();
            myButton.Show();
        }

        public override void Hide()
        {
            myText.Hide();
            myTextBox.Hide();
            myButton.Hide();
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myText);
            aPanel.Controls.Add(myTextBox);
            aPanel.Controls.Add(myButton);
        }

        public Label GetText()
        {
            return myText;
        }

        public TextBox GetTextBox()
        {
            return myTextBox;
        }

        public Button GetButton()
        {
            return myButton;
        }
    }
}
