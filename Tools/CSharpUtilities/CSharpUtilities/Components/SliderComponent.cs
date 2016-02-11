using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    public class SliderComponent : BaseComponent
    {
        private Label myLabel = new Label();

        private HScrollBar myScrollBar = new HScrollBar();

        private int myMaxValue;
        private int myMinValue;
        private int myStartValue;

        private Vector2<float> myDownScaleValue;

        private int myCurrentValue;
        private float myCurrentFloatValue;

        private string myText;
        private string myFormatedText;

        private bool myOneToOneScaleFlag = false;

        public SliderComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName, 
            float aMinValue, float aMaxValue, float aStartValue, bool aOneToOneFlag = false, 
            float aDownScaleMinValue = -1.0f, float aDownScaleMaxValue = 1.0f)
            : base(aLocation, aSize, aIO, aPanelName)
        {
            myOneToOneScaleFlag = aOneToOneFlag;

            myDownScaleValue.myX = aDownScaleMinValue;
            myDownScaleValue.myY = aDownScaleMaxValue;

            myMaxValue = (int)aMaxValue;
            myMinValue = (int)aMinValue;
            myStartValue = (int)aStartValue;

            if (myMaxValue < myMinValue) myMaxValue = myMinValue;
            if (myMinValue > myMaxValue) myMinValue = myMaxValue;
            if (myStartValue < myMinValue) myStartValue = myMinValue;
            if (myStartValue > myMaxValue) myStartValue = myMaxValue;

            myScrollBar.Maximum = myMaxValue;
            myScrollBar.Minimum = myMinValue;
            myScrollBar.Value = myStartValue;
            myScrollBar.LargeChange = 1;
            myScrollBar.TabIndex = 1;

            myCurrentFloatValue = CSharpUtilities.MathUtilities.Remap(myCurrentValue, myMinValue, myMaxValue, 
                myDownScaleValue.myX, myDownScaleValue.myY);
            float truncatedValue = (float)(Math.Truncate((double)myCurrentFloatValue * 100.0) / 100.0);
            myCurrentFloatValue = truncatedValue;

            if (aOneToOneFlag == false)
            {
                myFormatedText = myText + myCurrentValue.ToString();
                myLabel.Text = myFormatedText;
            }
            else
            {
                myFormatedText = myText + myCurrentFloatValue.ToString();
                myLabel.Text = myFormatedText;
            }
        }

        protected override void InitializeComponents(string aText)
        {
            myText = aText;
            int textSize = (aText.Length + 3) * 10;
            if(textSize > mySize.Width/2) textSize = mySize.Width/2;
            myLabel.Text = aText;
            myLabel.Location = new Point(myLocation.X, myLocation.Y);
            myLabel.Size = new Size(textSize, mySize.Height);

            myScrollBar.Location = new Point(myLocation.X + textSize, myLocation.Y);
            myScrollBar.Size = new Size(mySize.Width - textSize, mySize.Height);
            myScrollBar.ValueChanged += new EventHandler(this.ScrollValue_Changed);
        }

        public void AddSelectedValueChangedEvent(EventHandler aEvent)
        {
            myScrollBar.ValueChanged += new EventHandler(aEvent);
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);
            aPanel.Controls.Add(myScrollBar);
        }

        public override void Hide()
        {
            myLabel.Hide();
            myScrollBar.Hide();
        }

        public override void Show()
        {
            myLabel.Show();
            myScrollBar.Show();
        }

        private void ScrollValue_Changed(object sender, EventArgs e)
        {
            myCurrentValue = myScrollBar.Value;
            myCurrentFloatValue = CSharpUtilities.MathUtilities.Remap(myCurrentValue, myMinValue, myMaxValue, 
                myDownScaleValue.myX, myDownScaleValue.myY);
            float truncatedValue = (float)(Math.Truncate((double)myCurrentFloatValue * 100.0) / 100.0);
            myCurrentFloatValue = truncatedValue;
            if (myOneToOneScaleFlag == false)
            {
                myFormatedText = myText + myCurrentValue.ToString();
                myLabel.Text = myFormatedText;
            }
            else
            {
                myFormatedText = myText + myCurrentFloatValue.ToString();
                myLabel.Text = myFormatedText;
            }
        }

        public float GetValue()
        {
            if (myOneToOneScaleFlag == false)
            {
                return myCurrentValue;
            }
            return myCurrentFloatValue;
        }

        public void SetValue(float aValue)
        {
            myCurrentFloatValue = aValue;
            myCurrentValue = (int)MathUtilities.Remap(aValue, myDownScaleValue.myX, myDownScaleValue.myY, myMinValue, myMaxValue);

            myScrollBar.Value = myCurrentValue;
        } 
    }
}
