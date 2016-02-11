using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LogReader
{
    public partial class ToggleFilter : Form
    {
        private Filters[] myFilters;
        public ToggleFilter()
        {
            InitializeComponent();
        }

        public ToggleFilter(Form aOwner, Filters[] aFilterList)
        {
            this.Owner = aOwner;
            myFilters = aFilterList;
            InitializeComponent();
            if (myFilters != null)
            {
                ToggleFilter_CreateFilterCheckBoxes();
            }
        }

        private void ToggleFilter_CreateFilterCheckBoxes()
        {
            int numberOfFilterAdded = 0;
            for (int i = 0; i < myFilters.GetLength(0); ++i)
            {
                if (myFilters[i].myFilterName != null)
                {
                    ToggleFilteCheckBox.Items.Add(myFilters[i].myFilterName);
                    ToggleFilteCheckBox.SetItemChecked(numberOfFilterAdded, myFilters[i].myActiveBool);
                    ++numberOfFilterAdded;
                }
            }
        }

        private void Close_Btn_Click(object sender, EventArgs e)
        {
            LogReaderApp logReader = (LogReaderApp)this.Owner;
            this.Visible = false;
            logReader.RefreshLogText(myFilters);
            logReader.Visible = true;
            logReader.Activate();
            this.Dispose();
        }

        private void ToggleFilteCheckBox_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (ToggleFilteCheckBox.SelectedItem != null)
            {
                string selectedFilter = (string)ToggleFilteCheckBox.SelectedItem;

                for (int i = 0; i < myFilters.GetLength(0); ++i)
                {
                    if (myFilters[i].myFilterName == selectedFilter)
                    {
                        myFilters[i].myActiveBool = !myFilters[i].myActiveBool;
                        return;
                    }
                }
            }
        }
    }
}
