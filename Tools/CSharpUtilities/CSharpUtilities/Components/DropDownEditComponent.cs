using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace CSharpUtilities.Components
{
    public class DropDownComponent : BaseComponent
    {
        private Label myLabel = new Label();
        private ComboBox myDropDown = new ComboBox();

        public DropDownComponent(Point aLocation, Size aSize, IO.ComponentIO aIO, string aPanelName)
            : base(aLocation, aSize, aIO, aPanelName)
        {

        }

        protected override void InitializeComponents(string aText)
        {
            int textSize = aText.Length * 10;
            if (textSize > 60) textSize = 60;
            int sizePerObject = mySize.Width - textSize;
            myLabel.Text = aText;
            myLabel.Location = new Point(myLocation.X, myLocation.Y + 3);
            myLabel.Size = new Size(textSize, mySize.Height + 2);

            myDropDown.Location = new Point(myLocation.X + textSize, myLocation.Y);
            myDropDown.Size = new Size(sizePerObject, mySize.Height);
            myDropDown.DropDownStyle = ComboBoxStyle.DropDownList;
        }

        public override void Show()
        {
            myLabel.Show();
            myDropDown.Show();
        }

        public override void Hide()
        {
            myLabel.Hide();
            myDropDown.Hide();
        }

        public void AddSelectedIndexChangeEvent(EventHandler aEvent)
        {
            myDropDown.SelectedIndexChanged += new EventHandler(aEvent);
        }

        public override void BindToPanel(Panel aPanel)
        {
            aPanel.Controls.Add(myLabel);
            aPanel.Controls.Add(myDropDown);
        }

        public void AddItem(object aItem)
        {
            myDropDown.Items.Add(aItem);
        }

        public void RemoveItem(object aItem)
        {
            myDropDown.Items.Remove(aItem);
        }

        public ComboBox GetDropDown()
        {
            return myDropDown;
        }
    }
}
