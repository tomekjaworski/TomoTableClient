﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using CnC;

namespace IntelHEX
{
    public class IntelHEX16Storage
    {
        private MemoryMap mm;

        public IntelHEX16Storage(MemoryMap map)
        {
            this.mm = map;
        }

        public void Save(string file_name)
        {

            using (FileStream fs = File.Create(file_name))
            using (StreamWriter sw = new StreamWriter(fs, Encoding.ASCII))
            {
                int address = 0;
                int bytes_left = (int)this.mm.Size;
                byte[] row = new byte[16];
                while (bytes_left > 0)
                {
                    int checksum =  0;
                    int row_length = bytes_left > 16 ? 16 : bytes_left;
                    for (int i = 0; i < row_length; i++)
                    {
                        row[i] = this.mm.ReadByte((uint)(address + i));
                        checksum += row[i];
                    }

                    checksum += row_length;
                    checksum += (address & 0xFF);
                    checksum += ((address >> 8) & 0xFF);
                    checksum = -checksum;
                    checksum = checksum & 0x000000ff;

                    sw.Write(":");
                    sw.Write($"{row_length:X2}{address:X4}{00:X2}");
                    sw.Write(string.Join("", row.Select(x => x.ToString("X2"))));
                    sw.WriteLine($"{checksum:X2}");
                    bytes_left -= row_length;
                    address += row_length;
                }
                // EOF for hex
                sw.WriteLine($":{0:X2}{0:X4}{01:X2}{255:X2}");
            } // 2xusing

        }

        public void Load(string file_name)
        {
            try
            {
                string[] rows = File.ReadAllLines(file_name);

                UInt32 addr_high = 0;
                bool found_EOF = false;
                foreach (string row in rows)
                {
                    if (!row.StartsWith(":"))
                        throw new IntelHEXFormatException("':' expected at the beginning of a line");

                    //  . .   . .                               .
                    // :10010000214601360121470136007EFE09D2190140
                    // :10 0100 00 214601360121470136007EFE09D21901 40
                    uint byte_count = uint.Parse(row.Substring(1, 2), System.Globalization.NumberStyles.HexNumber);

                    uint addr_msb = uint.Parse(row.Substring(3, 2), System.Globalization.NumberStyles.HexNumber);
                    uint addr_lsb = uint.Parse(row.Substring(5, 2), System.Globalization.NumberStyles.HexNumber);
                    uint addr_low = addr_msb << 8 | addr_lsb;
                    uint type = uint.Parse(row.Substring(7, 2), System.Globalization.NumberStyles.HexNumber);
                    uint checksum = uint.Parse(row.Substring(row.Length - 2, 2), System.Globalization.NumberStyles.HexNumber);


                    byte[] bytes = new byte[byte_count];
                    for (int i = 0; i < byte_count; i++)
                        bytes[i] = byte.Parse(row.Substring(9 + i * 2, 2), System.Globalization.NumberStyles.HexNumber);

                    // checksum calculation and verification
                    byte chk = 0;
                    for (int i = 0; i < (row.Length - 1) / 2; i++)
                        chk += byte.Parse(row.Substring(1 + i * 2, 2), System.Globalization.NumberStyles.HexNumber);
                    if (chk != 0x00)
                        throw new IntelHEXFormatException("Checksum error");


                    if (type == 0x00) // data record
                    {
                        this.mm.Write(addr_high | addr_low, bytes, 0, (int)byte_count);
                        continue;
                    }
                    if (type == 0x01) // end of file
                    {
                        found_EOF = true;
                        break;
                    }
                    if (type == 0x04) // set high word of the address
                    {
                        addr_high = (UInt32)bytes[0] << 24;
                        addr_high |= (UInt32)bytes[1] << 16;
                        continue;
                    }

                    if (type == 0x03) // binary image entry point
                    {
                        uint entry_point = 0;
                        entry_point = entry_point << 8 | bytes[0];
                        entry_point = entry_point << 8 | bytes[1];
                        entry_point = entry_point << 8 | bytes[2];
                        entry_point = entry_point << 8 | bytes[3];
                        continue;
                    }

                    throw new IntelHEXFormatException(string.Format("type == 0x{0:2X} not implemented", type));
                }

                // have we found eof?
                if (!found_EOF)
                    throw new IntelHEXFormatException("End-Of-File command (0x01) not found at the bottom of the input file");

            }
            catch (MemoryMapException ex)
            {
                throw ex;
            }
            catch (Exception ex)
            {
                throw new IntelHEXFormatException(ex.Message, ex);
            }
        }

    }
}
