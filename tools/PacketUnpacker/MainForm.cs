using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PacketUnpacker
{
    public partial class MainForm : Form
    {
        Unpacker _unpacker = new ReadableStyleUnpacker();

        public MainForm()
        {
            InitializeComponent();
            outputTextBox.Text = _unpacker.UnpackPacket(inputTextBox.Text);
            readableOutputStyleToolStripMenuItem.Checked = true;
        }

        #region Event Handlers

        private void inputTextBox_TextChanged(object sender, EventArgs e)
        {
            outputTextBox.Text = _unpacker.UnpackPacket(inputTextBox.Text);
        }


        private void readableOutputStyleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _unpacker = new ReadableStyleUnpacker();
            outputTextBox.Text = _unpacker.UnpackPacket(inputTextBox.Text);
            readableOutputStyleToolStripMenuItem.Checked = true;
            codeOutputStyleToolStripMenuItem.Checked = false;
        }

        private void codeOutputStyleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _unpacker = new CodeStyleUnpacker();
            outputTextBox.Text = _unpacker.UnpackPacket(inputTextBox.Text);
            readableOutputStyleToolStripMenuItem.Checked = false;
            codeOutputStyleToolStripMenuItem.Checked = true;
        }

        #endregion
    }
}
