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

namespace LogReader
{
    public struct Filters
    {
        public string myFilterName;
        public bool myActiveBool;
    };

    public partial class LogReaderApp : Form
    {
        private OpenFileWindow myOpenWindow;
        private ToggleFilter myToggleFilterWindow;

        private string myFilePath = "";
        private string myFolderPath = "";
        private FileInfo myCurrentFileInfo;

        private int myNumberOfFiltes = 0;
        private Filters[] myFilters;
        
        public LogReaderApp()
        {
            myOpenWindow = new OpenFileWindow(this, myFolderPath);
            InitializeComponent();
            this.Name = "Log Reader";
            this.Text = this.Name + " v0.3";
        }

        private void Open_Btn_Click(object sender, EventArgs e)
        {
            myOpenWindow = new OpenFileWindow(this, myFolderPath);
            myOpenWindow.Activate();
            myOpenWindow.Visible = true;
        }

        public void SetFilePath(string aFilePath)
        {
            myFilePath = aFilePath;

            myCurrentFileInfo = new FileInfo(myFilePath);

            LogFileTextWindow.ReadOnly = false;
            LogFileTextWindow.Text = File.ReadAllText(myCurrentFileInfo.FullName);
            LogFileTextWindow.ReadOnly = true;

            string[] lines = File.ReadAllLines(myCurrentFileInfo.FullName);

            myFilters = new Filters[lines.GetLength(0)];

            for (int i = 0; i < lines.GetLength(0); ++i)
            {
                string currentFilterTag = "";
                bool currentFilteTagExists = false;
                int amountOfFoundTagStarts = 0;
                for (int j = 0; j < lines[i].Length; ++j)
                {
                    if (lines[i][j] == '[')
                    {
                        ++amountOfFoundTagStarts;
                    }
                    if (amountOfFoundTagStarts == 2)
                    {
                        if (lines[i][j] == '[')
                        {
                            continue;
                        }
                        if (lines[i][j] == ']')
                        {
                            break;
                        }
                        currentFilterTag += lines[i][j];
                    }
                }
                for (int k = 0; k < myNumberOfFiltes; ++k)
                {
                    if(myFilters[k].myFilterName == currentFilterTag)
                    {
                        currentFilteTagExists = true;
                        break;
                    }
                }
                if (currentFilteTagExists == false)
                {
                    myFilters[myNumberOfFiltes].myFilterName = currentFilterTag;
                    myFilters[myNumberOfFiltes].myActiveBool = true;
                    ++myNumberOfFiltes;
                }
            }
        }

        public void SetFolderPath(string aFolderPath)
        {
            myFolderPath = aFolderPath;
        }

        private void ToggleFilter_Btn_Click(object sender, EventArgs e)
        {
            myToggleFilterWindow = new ToggleFilter(this, myFilters);
            myToggleFilterWindow.Activate();
            myToggleFilterWindow.Visible = true;
        }

        public void RefreshLogText(Filters[] aFilterList)
        {
            myFilters = aFilterList;

            string[] lines = File.ReadAllLines(myCurrentFileInfo.FullName);
            string[] filteredLogText = new string[lines.GetLength(0)];
            int numberOfFilteredLines = 0;

            for (int i = 0; i < lines.GetLength(0); ++i)
            {
                string currentFilterTag = "";
                bool allowedLine = false;
                int amountOfFoundTagStarts = 0;
                for (int j = 0; j < lines[i].Length; ++j)
                {
                    if (lines[i][j] == '[')
                    {
                        ++amountOfFoundTagStarts;
                    }
                    if (amountOfFoundTagStarts == 2)
                    {
                        if (lines[i][j] == ']')
                        {
                            break;
                        }
                        currentFilterTag += lines[i][j];
                    }
                }
                for (int k = 0; k < myNumberOfFiltes; ++k)
                {
                    if (myFilters[k].myFilterName == currentFilterTag)
                    {
                        allowedLine = myFilters[k].myActiveBool;
                        break;
                    }
                }
                if (allowedLine == true)
                {
                    filteredLogText[numberOfFilteredLines] = lines[i] + "\n";
                    ++numberOfFilteredLines;
                }
            }

            LogFileTextWindow.ReadOnly = false;
            LogFileTextWindow.Text = "";
            for (int i = 0; i < numberOfFilteredLines; ++i)
            {
                LogFileTextWindow.Text += filteredLogText[i];
            }
            LogFileTextWindow.ReadOnly = true;
        }
    }
}
