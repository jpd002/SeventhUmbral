using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.IO.Compression;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PacketUnpacker
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();

            outputTextBox.Text = UnpackPacket(inputTextBox.Text);
        }

        private void inputTextBox_TextChanged(object sender, EventArgs e)
        {
            outputTextBox.Text = UnpackPacket(inputTextBox.Text);
        }

        private string PrintStreamContents(Stream stream)
        {
            var result = new StringBuilder();

            var outputBuffer = new byte[0x10];

            while (true)
            {
                int readAmount = stream.Read(outputBuffer, 0, outputBuffer.Length);
                if (readAmount == 0) break;
                for (int i = 0; i < readAmount; i++)
                {
                    result.AppendFormat("{0:x2} ", outputBuffer[i]);
                }
                for (int i = readAmount; i < 0x10; i++)
                {
                    result.Append("   ");
                }
                result.Append("         ");
                for (int i = 0; i < readAmount; i++)
                {
                    byte inputByte = outputBuffer[i];
                    char outputChar = '.';
                    if (char.IsLetterOrDigit((char)inputByte))
                    {
                        outputChar = (char)inputByte;
                    }
                    result.AppendFormat("{0}", outputChar);
                }
                result.AppendFormat("\r\n");
            }

            return result.ToString();
        }

        private string PrintPacketCommands(Stream stream)
        {
            var result = new StringBuilder();

            while (true)
            {
                var header = new byte[0x10];
                var headerReadSize = stream.Read(header, 0, header.Length);
                if (headerReadSize != 0x10) break;
                stream.Seek(-0x10, SeekOrigin.Current);

                UInt16 commandSize = (UInt16)((header[0]) | (header[1] << 8));
                var commandData = new byte[commandSize];
                stream.Read(commandData, 0, commandData.Length);

                result.AppendFormat("Command Size: {0} bytes\r\n", commandSize);
                using (var commandDataStream = new MemoryStream(commandData))
                {
                    result.AppendFormat(PrintStreamContents(commandDataStream));
                }
                result.Append("\r\n");
            }

            return result.ToString();
        }

        private string UnpackPacket(string inputString)
        {
            try
            {
                if (string.IsNullOrEmpty(inputString) || (inputString.Length & 1) != 0)
                {
                    throw new Exception("Invalid input packet.");
                }

                using (var packetDataStream = new MemoryStream())
                {
                    for (int i = 0; i < inputString.Length / 2; i++)
                    {
                        string byteValString = new string(new char[] { inputString[(i * 2) + 0], inputString[(i * 2) + 1] });
                        uint byteVal = Convert.ToUInt32(byteValString, 16);
                        packetDataStream.WriteByte((byte)byteVal);
                    }

                    packetDataStream.Seek(0, SeekOrigin.Begin);

                    var result = new StringBuilder();

                    while (true)
                    {
                        var header = new byte[0x10];
                        var headerReadSize = packetDataStream.Read(header, 0, header.Length);
                        if (headerReadSize != 0x10) break;

                        UInt16 subPacketSize = (UInt16)((header[4]) | (header[5] << 8));

                        result.AppendFormat("Sub Packet Size: {0} bytes\r\n\r\n", subPacketSize);

                        var subPacketData = new byte[subPacketSize - 0x10];
                        packetDataStream.Read(subPacketData, 0, subPacketData.Length);

                        using (MemoryStream compressedSubPacketDataStream = new MemoryStream(subPacketData))
                        {
                            compressedSubPacketDataStream.ReadByte();
                            compressedSubPacketDataStream.ReadByte();

                            using (var deflateStream = new DeflateStream(compressedSubPacketDataStream, CompressionMode.Decompress))
                            {
                                using (var subPacketDataStream = new MemoryStream())
                                {
                                    var buffer = new byte[0x200];
                                    while (true)
                                    {
                                        int readAmount = deflateStream.Read(buffer, 0, buffer.Length);
                                        if (readAmount == 0) break;
                                        subPacketDataStream.Write(buffer, 0, readAmount);
                                    }

                                    subPacketDataStream.Seek(0, SeekOrigin.Begin);

                                    result.Append(PrintPacketCommands(subPacketDataStream));
                                    result.Append("-------------------------------------------------------------------------------\r\n");
                                }
                            }
                        }
                    }

                    return result.ToString();
                }
            }
            catch (Exception exception)
            {
                return exception.Message;
            }
        }
    }
}
