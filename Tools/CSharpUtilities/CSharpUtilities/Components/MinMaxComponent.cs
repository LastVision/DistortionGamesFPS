using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Windows.Forms;

namespace CSharpUtilities.Components
{
    public class MinMaxComponent : BaseComponent
    {
        private Label myLabel = new Label();
        private NumericTextComponent myMinValue;
        private NumericTextComponent myMaxValue;

        public MinMaxComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName)
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

            myMinValue = new NumericTextComponent(new Point(myLocation.X + textSize, myLocation.Y), new Size(100, mySize.Height), myIO, myPanelName);
            myMaxValue = new NumericTextComponent(new Point(myLocation.X + textSize + 100, myLocation.Y), new Size(100, mySize.Height), myIO, myPanelName);
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);

            myMinValue.BindToPanel(aPanel);
            myMaxValue.BindToPanel(aPanel);
        }

        public override void Hide()
        {
            myLabel.Hide();
            myMinValue.Hide();
            myMaxValue.Hide();
        }

        public override void Show()
        {
            myLabel.Show();
            myMinValue.Show();
            myMaxValue.Show();
        }

        public void AddTextChangeEvent(EventHandler aEvent)
        {
            myMinValue.GetTextBox().TextChanged += new EventHandler(aEvent);
            myMaxValue.GetTextBox().TextChanged += new EventHandler(aEvent);
        }

        public void SetMinValue(float aValue)
        {
            myMinValue.GetTextBox().Text = aValue.ToString();
        }

        public float GetMinValue()
        {
            float value = 0;
            if (myMinValue.GetTextBox().Text != "") value = StringUtilities.ToFloat(myMinValue.GetTextBox().Text);
            return value;
        }

        public void SetMaxValue(float aValue)
        {
            myMaxValue.GetTextBox().Text = aValue.ToString();
        }

        public float GetMaxValue()
        {
            float value = 0;
            if (myMaxValue.GetTextBox().Text != "") value = StringUtilities.ToFloat(myMaxValue.GetTextBox().Text);
            return value;
        }
    }
}
