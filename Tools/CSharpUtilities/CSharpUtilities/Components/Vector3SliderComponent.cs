using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Windows.Forms;

namespace CSharpUtilities.Components
{
    public class Vector3SliderComponent : BaseComponent
    {
        private Label myLabel = new Label();

        private SliderComponent myXSlider;
        private SliderComponent myYSlider;
        private SliderComponent myZSlider;

        public Vector3SliderComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName, 
            float aMinValue, float aMaxValue, float aStartValue, bool aOneToOneScaleFlag = false, 
            string aXText = "X: ", string aYText ="Y: ", string aZText = "Z: ", 
            float aDownScaleMinValue = -1.0f, float aDownScaleMaxValue = 1.0f)
            : base(aLocation, aSize, aIO, aPanelName)
        {
            InitializeComponents(aIO.myIOElementName, aMinValue, aMaxValue, aStartValue, aOneToOneScaleFlag, 
                aXText, aYText, aZText, aDownScaleMinValue, aDownScaleMaxValue);
        }

        protected override void InitializeComponents(string aText)
        {
            int textSize = aText.Length * 10;
            if (textSize > mySize.Width) textSize = mySize.Width;
            myLabel.Text = aText;
            myLabel.Location = new Point(myLocation.X + ((mySize.Width - (textSize / 2)) / 2), myLocation.Y);
            myLabel.Size = new Size(textSize, 13);
        }

        protected void InitializeComponents(string aText, float aMinValue, float aMaxValue, float aStartValue,
            bool aOneToOneScaleFlag, string aXText, string aYText, string aZText, 
            float aDownScaleMinValue, float aDownScaleMaxValue)
        {
            myXSlider = new SliderComponent(new Point(myLocation.X, myLocation.Y + 15),
                new Size(175, 13), myIO, myPanelName, aMinValue, aMaxValue, aStartValue, aOneToOneScaleFlag, 
                aDownScaleMinValue, aDownScaleMaxValue);
            myYSlider = new SliderComponent(new Point(myLocation.X, myLocation.Y + 30),
                new Size(175, 13), myIO, myPanelName, aMinValue, aMaxValue, aStartValue, aOneToOneScaleFlag, 
                aDownScaleMinValue, aDownScaleMaxValue);
            myZSlider = new SliderComponent(new Point(myLocation.X, myLocation.Y + 45),
                new Size(175, 13), myIO, myPanelName, aMinValue, aMaxValue, aStartValue, aOneToOneScaleFlag, 
                aDownScaleMinValue, aDownScaleMaxValue);
        }

        public void AddSelectedValueChangedEvent(EventHandler aEvent)
        {
            myXSlider.AddSelectedValueChangedEvent(aEvent);
            myYSlider.AddSelectedValueChangedEvent(aEvent);
            myZSlider.AddSelectedValueChangedEvent(aEvent);
        }

        public void AddSelectedXValueChangedEvent(EventHandler aEvent)
        {
            myXSlider.AddSelectedValueChangedEvent(aEvent);
        }

        public void AddSelectedYValueChangedEvent(EventHandler aEvent)
        {
            myYSlider.AddSelectedValueChangedEvent(aEvent);
        }

        public void AddSelectedZValueChangedEvent(EventHandler aEvent)
        {
            myZSlider.AddSelectedValueChangedEvent(aEvent);
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);
            myXSlider.BindToPanel(aPanel);
            myYSlider.BindToPanel(aPanel);
            myZSlider.BindToPanel(aPanel);
        }

        public override void Hide()
        {
            myLabel.Hide();
            myXSlider.Hide();
            myYSlider.Hide();
            myZSlider.Hide();
        }

        public override void Show()
        {
            myLabel.Show();
            myXSlider.Show();
            myYSlider.Show();
            myZSlider.Show();
        }

        public float GetXValue()
        {
            return myXSlider.GetValue();
        }

        public void SetXValue(float aValue)
        {
            myXSlider.SetValue(aValue);
        }

        public float GetYValue()
        {
            return myYSlider.GetValue();
        }

        public void SetYValue(float aValue)
        {
            myYSlider.SetValue(aValue);
        }

        public float GetZValue()
        {
            return myZSlider.GetValue();
        }

        public void SetZValue(float aValue)
        {
            myZSlider.SetValue(aValue);
        }
    }
}
