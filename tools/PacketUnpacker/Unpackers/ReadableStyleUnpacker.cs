using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace PacketUnpacker
{
    class ReadableStyleUnpacker : Unpacker
    {
        protected override string PrintStreamContents(Stream stream)
        {
            var result = new StringBuilder();

            var outputBuffer = new byte[0x10];

            while (true)
            {
                int readAmount = stream.Read(outputBuffer, 0, outputBuffer.Length);
                if (readAmount == 0) break;
                for (int i = 0; i < readAmount; i++)
                {
                    result.AppendFormat("{0:X2} ", outputBuffer[i]);
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

        protected override string PrintCommandHeader(uint commandSize)
        {
            return string.Format("Command Size: 0x{0:X4} bytes\r\n", commandSize);
        }
    }
}
