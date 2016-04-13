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
using System.Diagnostics;
using System.Threading;

namespace Launcher
{
	public partial class Form1 : Form
	{
		private string myDocumentFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
		private string myConfigPath = "Data\\Setting\\SET_config.bin";
		private string myExePath = "Machina_Client.exe";
		private string myServerPath = "Machina_Server.exe";
		private string myLogo = "bin\\Data\\Resource\\Texture\\Logo\\T_launcher_logo.png";
		private string myGameName = "Machina";

		private Process myServer = new Process();
		private Process myGame = new Process();

		enum eResolutions
		{
			R1280x1024,
			R1600x900,
			R1920x1080,
			RAuto
		}

		enum eQuailty
		{
			Low,
			High
		}

		enum eMSAA
		{
			x1,
			x2,
			x4
		}

		public Form1()
		{

			InitializeComponent();

			string releaseConfig = myDocumentFolder + "\\Distortion Games\\" + myGameName + "\\" + myConfigPath;
			myConfigPath = releaseConfig;
			string subString = myDocumentFolder + "\\Distortion Games";
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}
			subString += "\\" + myGameName;
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}
			subString += "\\Data";
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}
			subString += "\\Setting";
			if (Directory.Exists(subString) == false)
			{
				Directory.CreateDirectory(subString);
			}

			//pictureBox1.Image = Image.FromFile(myLogo);
			pictureBox1.ImageLocation = Directory.GetCurrentDirectory() + "\\" + myLogo;
		}

		private void Form1_Load(object sender, EventArgs e)
		{

			myResolutionList.Items.Add("1280 x 1024");
			myResolutionList.Items.Add("1600 x 900");
			myResolutionList.Items.Add("1920 x 1080");
			myResolutionList.Items.Add("Automatic");
			myResolutionList.SelectedIndex = 3;

			myQualityList.Items.Add("Low");
			myQualityList.Items.Add("Medium");
			myQualityList.Items.Add("High");
			myQualityList.SelectedIndex = 1;

			//aaDropdown.Items.Add("MSAA x1");
			//aaDropdown.Items.Add("MSAA x2");
			//aaDropdown.Items.Add("MSAA x4");
			//aaDropdown.SelectedIndex = 2;

			//windowedCheckbox.Checked = true;

			if (File.Exists(myConfigPath))
			{
				using (BinaryReader reader = new BinaryReader(File.Open(myConfigPath, FileMode.Open)))
				{
					ReadResolutionFromFile(reader);
					ReadMSAAFromFile(reader);
					ReadWindowedFromFile(reader);
				}
			}

			myServer.StartInfo.FileName = myServerPath;
			myServer.StartInfo.WorkingDirectory = "bin\\";
			myServer.StartInfo.WindowStyle = ProcessWindowStyle.Minimized;
			if (File.Exists("bin\\" + myServerPath) == true)
			{
				if (IsProcessOpen(myServer.StartInfo.FileName) == false)
				{
					myServer.Start();
				}
			}
			else
			{
				MessageBox.Show("Could not find " + myServerPath + ".");
			}
		}

		public bool IsProcessOpen(string name)
		{
			foreach (Process runningProcess in Process.GetProcesses())
			{
				if (runningProcess.ProcessName.Contains(name) == true)
				{
					return true;
				}
			}
			return false;
		}

		private void button1_Click(object sender, EventArgs e)
		{
			using (BinaryWriter writer = new BinaryWriter(File.Open(myConfigPath, FileMode.Create)))
			{
				WriteResolutionToFile(writer);
				WriteMSAAToFile(writer);
				WriterWindowedToFile(writer);
				WriteQualityToFile(writer);
			}

			ProcessStartInfo processInfo = new ProcessStartInfo();
			processInfo.WorkingDirectory = "bin\\";
			processInfo.FileName = myExePath;

			if (File.Exists("bin\\" + myExePath) == true)
			{
				if (IsProcessOpen("Application_Release") == true)
				{
					MessageBox.Show("A instance of the game " + myGameName + " is already running.");
				}
				else
				{
					myGame.StartInfo.FileName = myExePath;
					myGame.StartInfo.WorkingDirectory = "bin\\";
					myGame.Start();
					myGame.WaitForInputIdle();

					Application.Exit();
				}
			}
			else
			{
				MessageBox.Show("Could not find " + myExePath + ".");
			}
		}

		private void WriteQualityToFile(BinaryWriter writer)
		{
			Int32 quality = myQualityList.SelectedIndex;
			writer.Write(quality);
		}

		void WriteResolutionToFile(BinaryWriter aWriter)
		{
			Int32 width = 800;
			Int32 height = 600;

			Screen scr = Screen.PrimaryScreen;
			switch (myResolutionList.SelectedIndex)
			{
				case (int)eResolutions.R1280x1024:
					width = 1280;
					height = 1024;
					break;
				case (int)eResolutions.R1600x900:
					width = 1600;
					height = 900;
					break;
				case (int)eResolutions.R1920x1080:
					width = 1920;
					height = 1080;
					break;
				default:
					width = scr.Bounds.Width;
					height = scr.Bounds.Height;

					if(width > 1920)
					{
						float ratio = (float)width / (float)height;
						width = 1920;
						height = (int)((float)width / ratio);
					}

					break;
			}

			aWriter.Write(width);
			aWriter.Write(height);
		}

		void WriteMSAAToFile(BinaryWriter aWriter)
		{
			Int32 MSAACount = 2;
			//switch (aaDropdown.SelectedIndex)
			//{
			//    case (int)eMSAA.x1:
			//        MSAACount = 1;
			//        break;
			//    case (int)eMSAA.x2:
			//        MSAACount = 2;
			//        break;
			//    case (int)eMSAA.x4:
			//        MSAACount = 4;
			//        break;
			//}

			aWriter.Write(MSAACount);
		}

		void WriterWindowedToFile(BinaryWriter aWriter)
		{
			//bool windowed = false;

			//if (windowed == true)
			//{
			//    aWriter.Write((Int32)1);
			//}
			//else
			//{
			aWriter.Write((Int32)0);
			//}
		}

		void ReadResolutionFromFile(BinaryReader aReader)
		{
			myResolutionList.SelectedIndex = 3;
		}

		void ReadMSAAFromFile(BinaryReader aReader)
		{
			//int msaa = aReader.ReadInt32();

			//if (msaa == 1)
			//{
			//    aaDropdown.SelectedIndex = 0;
			//}
			//else if (msaa == 2)
			//{
			//    aaDropdown.SelectedIndex = 1;
			//}
			//else if (msaa == 4)
			//{
			//    aaDropdown.SelectedIndex = 2;
			//}
		}

		void ReadWindowedFromFile(BinaryReader aReader)
		{
			//int windowed = aReader.ReadInt32();

			//if (windowed == 1)
			//{
			//    windowedCheckbox.Checked = true;
			//}
			//else
			//{
			//    windowedCheckbox.Checked = false;
			//}
		}

		private void windowedCheckbox_CheckedChanged(object sender, EventArgs e)
		{
			//if (windowedCheckbox.Checked == true)
			//{
			//    resolutionDropdown.Enabled = true;
			//}
			//else
			//{
			//    resolutionDropdown.Enabled = false;
			//}
		}
	}
}
