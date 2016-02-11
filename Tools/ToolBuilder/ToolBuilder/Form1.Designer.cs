namespace ToolBuilder
{
    partial class Tool_Window
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
            this.components = new System.ComponentModel.Container();
            this.Tool_Menu = new System.Windows.Forms.Panel();
            this.Tool_Window_Main = new System.Windows.Forms.Panel();
            this.Tool_Window_Property = new System.Windows.Forms.Panel();
            this.myTimer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // Tool_Menu
            // 
            this.Tool_Menu.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.Tool_Menu.Dock = System.Windows.Forms.DockStyle.Top;
            this.Tool_Menu.Location = new System.Drawing.Point(0, 0);
            this.Tool_Menu.Name = "Tool_Menu";
            this.Tool_Menu.Size = new System.Drawing.Size(1008, 36);
            this.Tool_Menu.TabIndex = 0;
            // 
            // Tool_Window_Main
            // 
            this.Tool_Window_Main.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Tool_Window_Main.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(44)))), ((int)(((byte)(44)))), ((int)(((byte)(44)))));
            this.Tool_Window_Main.Location = new System.Drawing.Point(0, 40);
            this.Tool_Window_Main.Name = "Tool_Window_Main";
            this.Tool_Window_Main.Size = new System.Drawing.Size(700, 520);
            this.Tool_Window_Main.TabIndex = 1;
            // 
            // Tool_Window_Property
            // 
            this.Tool_Window_Property.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Tool_Window_Property.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(44)))), ((int)(((byte)(44)))), ((int)(((byte)(44)))));
            this.Tool_Window_Property.Location = new System.Drawing.Point(704, 40);
            this.Tool_Window_Property.Name = "Tool_Window_Property";
            this.Tool_Window_Property.Size = new System.Drawing.Size(304, 520);
            this.Tool_Window_Property.TabIndex = 2;
            // 
            // myTimer
            // 
            this.myTimer.Interval = 1;
            this.myTimer.Tick += new System.EventHandler(this.OnTick);
            // 
            // Tool_Window
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.ClientSize = new System.Drawing.Size(1008, 561);
            this.Controls.Add(this.Tool_Window_Property);
            this.Controls.Add(this.Tool_Window_Main);
            this.Controls.Add(this.Tool_Menu);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.MinimumSize = new System.Drawing.Size(1024, 600);
            this.Name = "Tool_Window";
            this.Text = "Tool Builder";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel Tool_Menu;
        private System.Windows.Forms.Panel Tool_Window_Main;
        private System.Windows.Forms.Panel Tool_Window_Property;
        private System.Windows.Forms.Timer myTimer;
    }
}

