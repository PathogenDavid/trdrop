﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using YamlDotNet;

namespace trdropConfigCreator
{
    public partial class TrdropForm : MetroFramework.Forms.MetroForm
    {

        public TrdropForm()
        {
            InitializeComponent();
        }

        private void VideoTabControl_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Left)
            {
                int lastIndex = this.videoTabControl.TabCount - 1;
                if (this.videoTabControl.GetTabRect(lastIndex).Contains(e.Location))
                {
                    if(lastIndex < 3)
                    {
                        VideoOptionsTabPage videoTab = new VideoOptionsTabPage("Video " + (lastIndex + 1));
                        this.videoTabControl.TabPages.Insert(lastIndex,videoTab);
                        this.videoTabControl.SelectedIndex = lastIndex;
                    }
                    else
                    {
                        this.videoTabControl.SelectedIndex = lastIndex - 1;
                        MessageBox.Show("Trdrop currently only supports up to 3 videos. Sorry.");
                    }
                } 
            }
        }

        private void VideoTabControl_Selecting(object sender, TabControlCancelEventArgs e)
        {
            if (e.TabPageIndex == this.videoTabControl.TabCount - 1)
                e.Cancel = true;
        }

        private void videoTabControl_MouseUp(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Middle)
            {
                int tabCount = videoTabControl.TabCount;
                for (int i = 0; i < tabCount; ++i)
                {
                    Rectangle tabRect = videoTabControl.GetTabRect(i);
                    if(i != (tabCount - 1) && (tabCount > 2) && tabRect.Contains(e.Location))
                    {
                        videoTabControl.TabPages.RemoveAt(i);
                        for (int j = 0; j < videoTabControl.TabCount - 1; ++j)
                        {
                            videoTabControl.GetControl(j).Text = "Video " + (j + 1);
                        }
                        break;
                    }
                }
            }
        }

        private void videoTabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            TabControl tabControl = (TabControl)sender;
            int lastIndex = tabControl.TabCount - 1;

            if(tabControl.SelectedIndex == lastIndex)
            {
                tabControl.SelectedIndex = 0;
                tabControl.TabPages[0].Show();

            }


        }

        private void videoTabControl_Deselected(object sender, TabControlEventArgs e)
        {

        }

        private void videoOptionsTabPage1_Click(object sender, EventArgs e)
        {

        }
    }
}
