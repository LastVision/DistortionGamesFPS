using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.CompilerServices;

namespace CSharpUtilities
{
    public sealed class DL_Debug
    {
        private static DL_Debug ourInstance = null;

        private string myLogFileName = "Default.log";
        private bool myHasBeenCreated = false;

        private DL_Debug()
        {
        }

        public static DL_Debug GetInstance
        {
            get
            {
                if (ourInstance == null)
                {
                    ourInstance = new DL_Debug();
                }
                return ourInstance;
            }
        }

        public void Init(string aLogFile)
        {
            if (myHasBeenCreated == false)
            {
                myLogFileName = @"log/";
                myLogFileName += System.DateTime.Now.ToString("yyyy-MM-dd_HH_mm_ss_");
                myLogFileName += aLogFile;

                if (StringUtilities.StringHasExtension(myLogFileName) == false)
                {
                    
                    myLogFileName += ".log";
                }

                myHasBeenCreated = true;
            }
        }

        public void DL_MessageBox(string aMessage, string aTitle = "", 
                System.Windows.Forms.MessageBoxButtons aMessageButtonType = System.Windows.Forms.MessageBoxButtons.OK)
        {
            if (myHasBeenCreated == true)
            {
                System.Windows.Forms.MessageBox.Show(aMessage, aTitle, aMessageButtonType);
                WriteToLogFile(aMessage, aTitle);
            }
        }

        public void DL_ErrorMessage(string aMessage, [CallerFilePath] string aFile = "", 
                [CallerMemberName] string aMember = "", [CallerLineNumber] int aLineNumber = 0)
        {
            if (myHasBeenCreated == true)
            {
                System.Windows.Forms.MessageBox.Show(aMessage, "Error!", System.Windows.Forms.MessageBoxButtons.OK);
                WriteErrorToLogFile(aMessage, aFile, aMember, aLineNumber);
            }
        }

        private void WriteToLogFile(string aMessage, string aLogFilter)
        {
            string logMessage = "[" + aLogFilter + "]: " + aMessage + "\n";

            CreateFolderIfNotExists(myLogFileName);

            WriteToFile(logMessage);
        }

        private void WriteErrorToLogFile(string aMessage, string aFile, string aMember, int aLineNumber)
        {
            string logMessage = "[" + aMember + "]\nError in " + aMember +" at \nFile: " + aFile + " : Line (" + aLineNumber + ")\n";
            logMessage += "Message: \n'" + aMessage + "'\n";

            CreateFolderIfNotExists(myLogFileName);

            WriteToFile(logMessage);
        }

        private void WriteToFile(string aMessage)
        {
            FileStream fs = new FileStream(myLogFileName, FileMode.Append, FileAccess.Write, FileShare.Write);
            fs.Close();

            StreamWriter sw = new StreamWriter(myLogFileName, true, Encoding.ASCII);
            sw.Write(aMessage);
            sw.Close();
        }

        private void CreateFolderIfNotExists(string aPath)
        {
            if (System.IO.Directory.Exists(Path.GetDirectoryName(aPath)) == false)
            {
                System.IO.Directory.CreateDirectory(Path.GetDirectoryName(aPath));
            }
        }
    }
}
