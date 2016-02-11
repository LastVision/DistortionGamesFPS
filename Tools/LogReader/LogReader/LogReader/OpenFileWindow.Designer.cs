namespace LogReader
{
    partial class OpenFileWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.FolderPath_TxtBox = new System.Windows.Forms.TextBox();
            this.BrowseFolder_Btn = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.LogList = new System.Windows.Forms.ListBox();
            this.OpenFile_Btn = new System.Windows.Forms.Button();
            this.Cancel_Btn = new System.Windows.Forms.Button();
            this.FolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // FolderPath_TxtBox
            // 
            this.FolderPath_TxtBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.FolderPath_TxtBox.Location = new System.Drawing.Point(3, 6);
            this.FolderPath_TxtBox.Name = "FolderPath_TxtBox";
            this.FolderPath_TxtBox.Size = new System.Drawing.Size(176, 20);
            this.FolderPath_TxtBox.TabIndex = 0;
            // 
            // BrowseFolder_Btn
            // 
            this.BrowseFolder_Btn.Dock = System.Windows.Forms.DockStyle.Right;
            this.BrowseFolder_Btn.Location = new System.Drawing.Point(212, 0);
            this.BrowseFolder_Btn.Name = "BrowseFolder_Btn";
            this.BrowseFolder_Btn.Size = new System.Drawing.Size(72, 36);
            this.BrowseFolder_Btn.TabIndex = 1;
            this.BrowseFolder_Btn.Text = "Browse";
            this.BrowseFolder_Btn.UseVisualStyleBackColor = true;
            this.BrowseFolder_Btn.Click += new System.EventHandler(this.BrowseFolder_Btn_Click);
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.LogList);
            this.panel1.Location = new System.Drawing.Point(3, 32);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(281, 137);
            this.panel1.TabIndex = 2;
            // 
            // LogList
            // 
            this.LogList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.LogList.FormattingEnabled = true;
            this.LogList.Location = new System.Drawing.Point(4, 4);
            this.LogList.Name = "LogList";
            this.LogList.Size = new System.Drawing.Size(274, 121);
            this.LogList.TabIndex = 0;
            this.LogList.SelectedIndexChanged += new System.EventHandler(this.LogList_SelectedIndexChanged);
            // 
            // OpenFile_Btn
            // 
            this.OpenFile_Btn.Dock = System.Windows.Forms.DockStyle.Left;
            this.OpenFile_Btn.Location = new System.Drawing.Point(0, 0);
            this.OpenFile_Btn.Name = "OpenFile_Btn";
            this.OpenFile_Btn.Size = new System.Drawing.Size(139, 36);
            this.OpenFile_Btn.TabIndex = 3;
            this.OpenFile_Btn.Text = "Open File";
            this.OpenFile_Btn.UseVisualStyleBackColor = true;
            this.OpenFile_Btn.Click += new System.EventHandler(this.OpenFile_Btn_Click);
            // 
            // Cancel_Btn
            // 
            this.Cancel_Btn.Dock = System.Windows.Forms.DockStyle.Right;
            this.Cancel_Btn.Location = new System.Drawing.Point(145, 0);
            this.Cancel_Btn.Name = "Cancel_Btn";
            this.Cancel_Btn.Size = new System.Drawing.Size(139, 36);
            this.Cancel_Btn.TabIndex = 4;
            this.Cancel_Btn.Text = "Cancel";
            this.Cancel_Btn.UseVisualStyleBackColor = true;
            this.Cancel_Btn.Click += new System.EventHandler(this.Cancel_Btn_Click);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.FolderPath_TxtBox);
            this.panel2.Controls.Add(this.BrowseFolder_Btn);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(284, 36);
            this.panel2.TabIndex = 5;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.OpenFile_Btn);
            this.panel3.Controls.Add(this.Cancel_Btn);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(0, 175);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(284, 36);
            this.panel3.TabIndex = 1;
            // 
            // OpenFileWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 211);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.MinimumSize = new System.Drawing.Size(300, 250);
            this.Name = "OpenFileWindow";
            this.Text = "Open Log";
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox FolderPath_TxtBox;
        private System.Windows.Forms.Button BrowseFolder_Btn;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button OpenFile_Btn;
        private System.Windows.Forms.Button Cancel_Btn;
        private System.Windows.Forms.FolderBrowserDialog FolderBrowserDialog;
        private System.Windows.Forms.ListBox LogList;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
    }
}