using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    public class Vector3Component : BaseComponent
    {
        private Label myLabel = new Label();

        private NumericTextComponent myXText;
        private NumericTextComponent myYText;
        private NumericTextComponent myZText;

        public Vector3Component(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName)
            : base(aLocation, aSize, aIO, aPanelName)
        {

        }

        protected override void InitializeComponents(string aText)
        {
            int textSize = aText.Length * 10;
            if (textSize > 60) textSize = 60;
            myLabel.Text = aText;
            myLabel.Location = new Point(myLocation.X, myLocation.Y);
            myLabel.Size = new Size(textSize, mySize.Height);

            myXText = new NumericTextComponent(new Point(myLocation.X + textSize, myLocation.Y), new Size(60, mySize.Height), myIO, myPanelName);
            myYText = new NumericTextComponent(new Point(myLocation.X + textSize + 60, myLocation.Y), new Size(60, mySize.Height), myIO, myPanelName);
            myZText = new NumericTextComponent(new Point(myLocation.X + textSize + 120, myLocation.Y), new Size(60, mySize.Height), myIO, myPanelName);

        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);

            myXText.BindToPanel(aPanel);
            myYText.BindToPanel(aPanel);
            myZText.BindToPanel(aPanel);

        }

        public override void Show()
        {
            myLabel.Show();
            myXText.Show();
            myYText.Show();
            myZText.Show();
        }

        public override void Hide()
        {
            myLabel.Hide();
            myXText.Hide();
            myYText.Hide();
            myZText.Hide();
        }

        public void AddTextChangeEvent(EventHandler aEvent)
        {
            myXText.GetTextBox().TextChanged += new EventHandler(aEvent);
            myYText.GetTextBox().TextChanged += new EventHandler(aEvent);
            myZText.GetTextBox().TextChanged += new EventHandler(aEvent);
        }

        public float GetX()
        {
            float value = 0;
            if (myXText.GetTextBox().Text != "") value = StringUtilities.ToFloat(myXText.GetTextBox().Text);
            return value;
        }

        public TextBox GetXTextBox()
        {
            return myXText.GetTextBox();
        }

        public float GetY()
        {
            float value = 0;
            if (myYText.GetTextBox().Text != "") value = StringUtilities.ToFloat(myYText.GetTextBox().Text);
            return value;
        }

        public TextBox GetYTextBox()
        {
            return myYText.GetTextBox();
        }

        public float GetZ()
        {
            float value = 0;
            if (myZText.GetTextBox().Text != "") value = StringUtilities.ToFloat(myZText.GetTextBox().Text);
            return value;
        }

        public TextBox GetZTextBox()
        {
            return myZText.GetTextBox();
        }

        public void SetPosition(string aX, string aY, string aZ)
        {
            myXText.GetTextBox().Text = aX;
            myYText.GetTextBox().Text = aY;
            myZText.GetTextBox().Text = aZ;
        }

        public void SetPosition(float aX, float aY, float aZ)
        {
            myXText.GetTextBox().Text = aX.ToString();
            myYText.GetTextBox().Text = aY.ToString();
            myZText.GetTextBox().Text = aZ.ToString();
        }

        public void SetPosition(Vector3<float> aPosition)
        {
            myXText.GetTextBox().Text = aPosition.myX.ToString();
            myYText.GetTextBox().Text = aPosition.myY.ToString();
            myZText.GetTextBox().Text = aPosition.myZ.ToString();
        }
    }
}
