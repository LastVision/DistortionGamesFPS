namespace LogReader
{
    partial class ToggleFilter
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.ToggleFilteCheckBox = new System.Windows.Forms.CheckedListBox();
            this.Close_Btn = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.ToggleFilteCheckBox);
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(164, 212);
            this.panel1.TabIndex = 0;
            // 
            // ToggleFilteCheckBox
            // 
            this.ToggleFilteCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ToggleFilteCheckBox.FormattingEnabled = true;
            this.ToggleFilteCheckBox.Location = new System.Drawing.Point(1, 8);
            this.ToggleFilteCheckBox.Name = "ToggleFilteCheckBox";
            this.ToggleFilteCheckBox.Size = new System.Drawing.Size(161, 184);
            this.ToggleFilteCheckBox.TabIndex = 0;
            this.ToggleFilteCheckBox.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.ToggleFilteCheckBox_ItemCheck);
            // 
            // Close_Btn
            // 
            this.Close_Btn.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.Close_Btn.Location = new System.Drawing.Point(0, 218);
            this.Close_Btn.Name = "Close_Btn";
            this.Close_Btn.Size = new System.Drawing.Size(164, 23);
            this.Close_Btn.TabIndex = 1;
            this.Close_Btn.Text = "Close";
            this.Close_Btn.UseVisualStyleBackColor = true;
            this.Close_Btn.Click += new System.EventHandler(this.Close_Btn_Click);
            // 
            // ToggleFilter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(164, 241);
            this.Controls.Add(this.Close_Btn);
            this.Controls.Add(this.panel1);
            this.MinimumSize = new System.Drawing.Size(180, 280);
            this.Name = "ToggleFilter";
            this.Text = "ToggleFilter";
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button Close_Btn;
        private System.Windows.Forms.CheckedListBox ToggleFilteCheckBox;
    }
}