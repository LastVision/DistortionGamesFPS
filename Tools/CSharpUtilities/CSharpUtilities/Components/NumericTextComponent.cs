using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    public class NumericTextComponent : BaseComponent
    {
        private Label myLabel = new Label();
        private TextBox myTextBox = new TextBox();

        private bool myDigitOnlyFlag;

        public NumericTextComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName, bool aDigitOnlyFlag = true)
            : base(aLocation, aSize, aIO, aPanelName)
        {
            myDigitOnlyFlag = aDigitOnlyFlag;
        }

        protected override void InitializeComponents(string aText)
        {
            int textSize = aText.Length * 10;
            if (textSize > 60) textSize = 60;
            int sizePerObject = mySize.Width - textSize;

            myLabel.Text = aText;
            myLabel.Location = new Point(myLocation.X, myLocation.Y);
            myLabel.Size = new Size(textSize, mySize.Height);


            myTextBox.Location = new Point(myLocation.X + textSize, myLocation.Y - 3);
            myTextBox.Size = new Size(sizePerObject, mySize.Height);
            myTextBox.KeyPress += new KeyPressEventHandler(TextBox_KeyPress);
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);
            aPanel.Controls.Add(myTextBox);
        }

        public override void Hide()
        {
            myLabel.Hide();
            myTextBox.Hide();
        }

        public override void Show()
        {
            myLabel.Show();
            myTextBox.Show();
        }

        public void AddTextChangeEvent(EventHandler aEvent)
        {
            myTextBox.TextChanged += new EventHandler(aEvent);
        }

        public TextBox GetTextBox()
        {
            return myTextBox;
        }

        private void TextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (myDigitOnlyFlag == true)
            {
                if (char.IsControl(e.KeyChar) == false && char.IsDigit(e.KeyChar) == false 
                    && e.KeyChar != '.' && e.KeyChar != '-')
                {
                    e.Handled = true;
                }
                if (e.KeyChar == '.' && (sender as TextBox).Text.IndexOf('.') > -1)
                {
                    e.Handled = true;
                }
                if (e.KeyChar == '-' && (sender as TextBox).Text.IndexOf('-') > -1)
                {
                    e.Handled = true;
                }
            }
        }
    }
}
