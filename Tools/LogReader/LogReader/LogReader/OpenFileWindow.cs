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
    public partial class OpenFileWindow : Form
    {
        private string myCurrentLogFile = "";
        private string myCurrentPath = "";
        public OpenFileWindow()
        {

            InitializeComponent();
        }

        public OpenFileWindow(Form aOwner, string aFilePathToStartAt)
        {
            this.Owner = aOwner;
            InitializeComponent();
            myCurrentPath = aFilePathToStartAt;
            if (myCurrentPath != "")
            {
                FolderPath_TxtBox.Text = myCurrentPath;
                FolderBrowserDialog.SelectedPath = myCurrentPath;
                RefreshLogFiles();
            }
        }

        private void Cancel_Btn_Click(object sender, EventArgs e)
        {
            LogReaderApp logReader = (LogReaderApp)this.Owner;
            logReader.SetFolderPath(myCurrentPath);

            this.Visible = false;

            logReader.Activate();
            logReader.Visible = true;

            this.Dispose();
        }

        private void BrowseFolder_Btn_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog.ShowDialog();
            FolderPath_TxtBox.Text = FolderBrowserDialog.SelectedPath;

            myCurrentPath = FolderBrowserDialog.SelectedPath;

            DirectoryInfo folderDirectory = new DirectoryInfo(FolderBrowserDialog.SelectedPath);

            FileInfo[] filesInFolder = folderDirectory.GetFiles();

            FileInfo[] logFiles = new FileInfo[filesInFolder.GetLength(0) + 1];

            int numberOfLogFiles = 0;

            for (int i = 0; i < filesInFolder.GetLength(0); ++i)
            {
                if (filesInFolder[i].Extension == ".txt" || filesInFolder[i].Extension == ".log")
                {
                    logFiles[numberOfLogFiles] = filesInFolder[i];
                    ++numberOfLogFiles;
                }
            }

            for (int i = 0; i < logFiles.GetLength(0); ++i)
            {
                if (logFiles[i] != null)
                {
                    LogList.Items.Add(logFiles[i].Name);

                }
            }

        }

        private void RefreshLogFiles()
        {
            DirectoryInfo folderDirectory = new DirectoryInfo(myCurrentPath);

            FileInfo[] filesInFolder = folderDirectory.GetFiles();

            FileInfo[] logFiles = new FileInfo[filesInFolder.GetLength(0) + 1];

            int numberOfLogFiles = 0;

            for (int i = 0; i < filesInFolder.GetLength(0); ++i)
            {
                if (filesInFolder[i].Extension == ".txt" || filesInFolder[i].Extension == ".log")
                {
                    logFiles[numberOfLogFiles] = filesInFolder[i];
                    ++numberOfLogFiles;
                }
            }

            for (int i = 0; i < logFiles.GetLength(0); ++i)
            {
                if (logFiles[i] != null)
                {
                    LogList.Items.Add(logFiles[i].Name);

                }
            }
        }

        private void LogList_SelectedIndexChanged(object sender, EventArgs e)
        {
            myCurrentLogFile = (string)LogList.SelectedItem;
        }

        private void OpenFile_Btn_Click(object sender, EventArgs e)
        {
            LogReaderApp logReader = (LogReaderApp)this.Owner;
            logReader.SetFilePath(myCurrentPath + "\\" + myCurrentLogFile);
            logReader.SetFolderPath(myCurrentPath);

            this.Visible = false;

            logReader.Activate();
            logReader.Visible = true;

            this.Dispose();
        }

    }
}
