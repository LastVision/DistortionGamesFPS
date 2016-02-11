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
using System.Text.RegularExpressions;


namespace LogReader
{
    public partial class LogReader : Form
    {
        public LogReader()
        {
            InitializeComponent();
            MaximizeBox = false;

            treeView1.Nodes.Clear();
            DirectoryInfo dirInfo = new DirectoryInfo(@"Logs");
            if (dirInfo.Exists)
            {
                treeView1.AfterSelect += treeView1_AfterSelect;
                BuildTree(dirInfo, treeView1.Nodes);
            }
            treeView1.ExpandAll();


      
        }

        private void tabPage1_Click(object sender, EventArgs e)
        {

        }

        private void tabPage2_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            treeView1.Nodes.Clear();
            DirectoryInfo dirInfo = new DirectoryInfo(@"Logs");
            if (dirInfo.Exists)
            {
                treeView1.AfterSelect += treeView1_AfterSelect;
                BuildTree(dirInfo, treeView1.Nodes);
            }
            treeView1.ExpandAll();
        }

        private void BuildTree(DirectoryInfo directoryInfo, TreeNodeCollection addInMe)
        {
          
            TreeNode curNode = null;
            if (addInMe.ContainsKey(directoryInfo.FullName) == false)
                curNode = addInMe.Add(directoryInfo.FullName, directoryInfo.Name);
            else
                curNode = addInMe.Find(directoryInfo.FullName, false)[0];

            foreach (FileInfo file in directoryInfo.GetFiles())
            {
                if (curNode.Nodes.ContainsKey(file.FullName) == false)
                {
                    curNode.Nodes.Add(file.FullName, file.Name);
                }
            }
            foreach (DirectoryInfo subdir in directoryInfo.GetDirectories())
            {
                BuildTree(subdir, curNode.Nodes);
            }
        }

        private void treeView1_BeforeSelect(object sender, TreeViewEventArgs e)
        {

        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (treeView1.SelectedNode == null)
                return;

            List<string> updateTab = new List<string> { };
            List<string> engineTab = new List<string> { };
            List<string> renderTab = new List<string> { };
            List<string> messageTab = new List<string> { };
            List<string> physicsTab = new List<string> { };
            List<string> resourceTab = new List<string> { };
            List<string> assertTab = new List<string> { };
            List<string> fbxTab = new List<string> { };

            

            assertTab.Clear();
            renderTab.Clear();
            engineTab.Clear();
            updateTab.Clear();
            physicsTab.Clear();
            messageTab.Clear();
            resourceTab.Clear();
            fbxTab.Clear();

            try
            {
                StreamReader myStreamReader = new StreamReader(e.Node.Name);

                var paneContent = new StringBuilder();
                bool lineFound = false;

                foreach (string line in File.ReadLines(e.Node.Name))
                {
                    DateTime time = DateTime.Now;
                    int timePartLength = line.IndexOf(']');

                    if (timePartLength != -1)
                    {
                        string timePart = line.Substring(0, timePartLength+1);
                        time = DateTime.ParseExact(timePart, "[H:mm:ss]", null);

                    }

                    if (line.Contains("Message]"))
                    {
                        messageTab.Add(line);
                    }
                    else if (line.Contains("[FBX"))
                    {
                        fbxTab.Add(line);
                    }
                    else if (line.Contains("Render]"))
                    {
                        renderTab.Add(line);
                    }
                    else if (line.Contains("Engine]"))
                    {
                        engineTab.Add(line);
                    }
                    else if (line.Contains("Resource]"))
                    {
                        resourceTab.Add(line);
                    }
                    else if (line.Contains("Update]"))
                    {
                        updateTab.Add(line);
                    }
                    else if (line.Contains("Physics]"))
                    {
                        physicsTab.Add(line);
                    }
                    else if (line.Contains("Assert at:"))
                    {
                        assertTab.Add(line);
                    }
                    else if (line.Contains("File:"))
                    {
                        assertTab.Add(line);
                    }
                    else if (line.Contains("Line:"))
                    {
                        assertTab.Add(line);
                    }
                    else if (line.Contains("Function:"))
                    {
                        assertTab.Add(line);
                    }
                    else if (line.Contains("Message:"))
                    {
                        assertTab.Add(line);
                    }
                }


                foreach (string line in File.ReadLines(e.Node.Name))
                {
                    if (line.Contains("CallStack:"))
                    {
                        lineFound = !lineFound;
                    }
                    else
                    {
                        if (lineFound == true)
                        {
                            assertTab.Add(line);
                        }
                    }
                }
                richTextBox1.Lines = messageTab.ToArray();
                richTextBox2.Lines = renderTab.ToArray();
                richTextBox3.Lines = updateTab.ToArray();
                richTextBox4.Lines = resourceTab.ToArray();
                richTextBox5.Lines = physicsTab.ToArray();
                richTextBox6.Lines = engineTab.ToArray();
                richTextBox7.Lines = assertTab.ToArray();
                richTextBox8.Lines = fbxTab.ToArray();

                myStreamReader.Close();
            }
            catch (UnauthorizedAccessException)
            { }
            catch (IOException ex)
            {
                MessageBox.Show(ex.ToString());
            }

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
        }

        private void LogReader_Load(object sender, EventArgs e)
        {
            timer1.Start();
        }



        private void richTextBox4_TextChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox7_TextChanged(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            DirectoryInfo dirInfo = new DirectoryInfo(@"Logs");
            if (dirInfo.Exists)
            {
                treeView1.AfterSelect += treeView1_AfterSelect;
                BuildTree(dirInfo, treeView1.Nodes);
            }
        }

        private void toolStripContainer1_TopToolStripPanel_Click(object sender, EventArgs e)
        {

        }

        private void richTextBox8_TextChanged(object sender, EventArgs e)
        {

        }

     
    }
}
