namespace LogReader
{
    partial class LogReaderApp
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
            this.Open_Btn = new System.Windows.Forms.Button();
            this.ToggleFilter_Btn = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.LogFileTextWindow = new System.Windows.Forms.RichTextBox();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // Open_Btn
            // 
            this.Open_Btn.Location = new System.Drawing.Point(12, 12);
            this.Open_Btn.Name = "Open_Btn";
            this.Open_Btn.Size = new System.Drawing.Size(75, 23);
            this.Open_Btn.TabIndex = 0;
            this.Open_Btn.Text = "Open";
            this.Open_Btn.UseVisualStyleBackColor = true;
            this.Open_Btn.Click += new System.EventHandler(this.Open_Btn_Click);
            // 
            // ToggleFilter_Btn
            // 
            this.ToggleFilter_Btn.Location = new System.Drawing.Point(93, 12);
            this.ToggleFilter_Btn.Name = "ToggleFilter_Btn";
            this.ToggleFilter_Btn.Size = new System.Drawing.Size(75, 23);
            this.ToggleFilter_Btn.TabIndex = 1;
            this.ToggleFilter_Btn.Text = "Toggle Filter";
            this.ToggleFilter_Btn.UseVisualStyleBackColor = true;
            this.ToggleFilter_Btn.Click += new System.EventHandler(this.ToggleFilter_Btn_Click);
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.LogFileTextWindow);
            this.panel1.Location = new System.Drawing.Point(12, 41);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(263, 327);
            this.panel1.TabIndex = 2;
            // 
            // LogFileTextWindow
            // 
            this.LogFileTextWindow.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.LogFileTextWindow.Location = new System.Drawing.Point(0, 0);
            this.LogFileTextWindow.Name = "LogFileTextWindow";
            this.LogFileTextWindow.Size = new System.Drawing.Size(259, 322);
            this.LogFileTextWindow.TabIndex = 0;
            this.LogFileTextWindow.Text = "";
            this.LogFileTextWindow.WordWrap = false;
            // 
            // LogReaderApp
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 381);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.ToggleFilter_Btn);
            this.Controls.Add(this.Open_Btn);
            this.MinimumSize = new System.Drawing.Size(300, 420);
            this.Name = "LogReaderApp";
            this.Text = "LogReader";
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button Open_Btn;
        private System.Windows.Forms.Button ToggleFilter_Btn;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.RichTextBox LogFileTextWindow;
    }
}

