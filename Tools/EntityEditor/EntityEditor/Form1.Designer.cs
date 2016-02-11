namespace EntityEditor
{
    partial class EntityEditorForm
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
            this.EntityPanel = new System.Windows.Forms.Panel();
            this.EntityContentList = new System.Windows.Forms.ListBox();
            this.EntityPanelMenu = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newEntityToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openEntityToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.saveEntityToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openEntityToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addComponentToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeComponentToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openEntityFile = new System.Windows.Forms.OpenFileDialog();
            this.saveEntityFile = new System.Windows.Forms.SaveFileDialog();
            this.PropertyPanel = new System.Windows.Forms.Panel();
            this.EntityPanel.SuspendLayout();
            this.EntityPanelMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // EntityPanel
            // 
            this.EntityPanel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.EntityPanel.BackColor = System.Drawing.Color.White;
            this.EntityPanel.Controls.Add(this.EntityContentList);
            this.EntityPanel.Controls.Add(this.EntityPanelMenu);
            this.EntityPanel.Location = new System.Drawing.Point(0, 0);
            this.EntityPanel.Name = "EntityPanel";
            this.EntityPanel.Size = new System.Drawing.Size(360, 511);
            this.EntityPanel.TabIndex = 0;
            // 
            // EntityContentList
            // 
            this.EntityContentList.Dock = System.Windows.Forms.DockStyle.Top;
            this.EntityContentList.FormattingEnabled = true;
            this.EntityContentList.Location = new System.Drawing.Point(0, 24);
            this.EntityContentList.Name = "EntityContentList";
            this.EntityContentList.Size = new System.Drawing.Size(360, 485);
            this.EntityContentList.TabIndex = 1;
            this.EntityContentList.SelectedIndexChanged += new System.EventHandler(this.EntityContentList_SelectedIndexChanged);
            // 
            // EntityPanelMenu
            // 
            this.EntityPanelMenu.BackColor = System.Drawing.Color.LightGray;
            this.EntityPanelMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.openEntityToolStripMenuItem});
            this.EntityPanelMenu.Location = new System.Drawing.Point(0, 0);
            this.EntityPanelMenu.Name = "EntityPanelMenu";
            this.EntityPanelMenu.Size = new System.Drawing.Size(360, 24);
            this.EntityPanelMenu.TabIndex = 0;
            this.EntityPanelMenu.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newEntityToolStripMenuItem,
            this.openEntityToolStripMenuItem1,
            this.saveEntityToolStripMenuItem1,
            this.saveAsToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newEntityToolStripMenuItem
            // 
            this.newEntityToolStripMenuItem.Name = "newEntityToolStripMenuItem";
            this.newEntityToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.newEntityToolStripMenuItem.Text = "New Entity";
            this.newEntityToolStripMenuItem.Click += new System.EventHandler(this.newEntityToolStripMenuItem_Click);
            // 
            // openEntityToolStripMenuItem1
            // 
            this.openEntityToolStripMenuItem1.Name = "openEntityToolStripMenuItem1";
            this.openEntityToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.openEntityToolStripMenuItem1.Text = "Open Entity";
            this.openEntityToolStripMenuItem1.Click += new System.EventHandler(this.openEntityToolStripMenuItem1_Click);
            // 
            // saveEntityToolStripMenuItem1
            // 
            this.saveEntityToolStripMenuItem1.Name = "saveEntityToolStripMenuItem1";
            this.saveEntityToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.saveEntityToolStripMenuItem1.Text = "Save Entity";
            this.saveEntityToolStripMenuItem1.Click += new System.EventHandler(this.saveEntityToolStripMenuItem1_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.saveAsToolStripMenuItem.Text = "Save As..";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // openEntityToolStripMenuItem
            // 
            this.openEntityToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addComponentToolStripMenuItem,
            this.removeComponentToolStripMenuItem});
            this.openEntityToolStripMenuItem.Name = "openEntityToolStripMenuItem";
            this.openEntityToolStripMenuItem.Size = new System.Drawing.Size(88, 20);
            this.openEntityToolStripMenuItem.Text = "Components";
            // 
            // addComponentToolStripMenuItem
            // 
            this.addComponentToolStripMenuItem.Name = "addComponentToolStripMenuItem";
            this.addComponentToolStripMenuItem.Size = new System.Drawing.Size(184, 22);
            this.addComponentToolStripMenuItem.Text = "Add Component";
            this.addComponentToolStripMenuItem.Click += new System.EventHandler(this.addComponentToolStripMenuItem_Click);
            // 
            // removeComponentToolStripMenuItem
            // 
            this.removeComponentToolStripMenuItem.Name = "removeComponentToolStripMenuItem";
            this.removeComponentToolStripMenuItem.Size = new System.Drawing.Size(184, 22);
            this.removeComponentToolStripMenuItem.Text = "Remove Component";
            this.removeComponentToolStripMenuItem.Click += new System.EventHandler(this.removeComponentToolStripMenuItem_Click);
            // 
            // PropertyPanel
            // 
            this.PropertyPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.PropertyPanel.BackColor = System.Drawing.SystemColors.ControlLight;
            this.PropertyPanel.Location = new System.Drawing.Point(360, 0);
            this.PropertyPanel.Name = "PropertyPanel";
            this.PropertyPanel.Size = new System.Drawing.Size(366, 511);
            this.PropertyPanel.TabIndex = 1;
            // 
            // EntityEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(726, 511);
            this.Controls.Add(this.PropertyPanel);
            this.Controls.Add(this.EntityPanel);
            this.MainMenuStrip = this.EntityPanelMenu;
            this.Name = "EntityEditorForm";
            this.Text = "Form1";
            this.EntityPanel.ResumeLayout(false);
            this.EntityPanel.PerformLayout();
            this.EntityPanelMenu.ResumeLayout(false);
            this.EntityPanelMenu.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel EntityPanel;
        private System.Windows.Forms.MenuStrip EntityPanelMenu;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openEntityToolStripMenuItem;
        private System.Windows.Forms.OpenFileDialog openEntityFile;
        private System.Windows.Forms.ListBox EntityContentList;
        private System.Windows.Forms.ToolStripMenuItem newEntityToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openEntityToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem saveEntityToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem addComponentToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeComponentToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.SaveFileDialog saveEntityFile;
        private System.Windows.Forms.Panel PropertyPanel;
    }
}

