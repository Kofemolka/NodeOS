using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileSplit
{
    class Program
    {
        const int chunkSize = 512;

        static void Main(string[] args)
        {
            if (args.Count() != 1)
            {
                return;
            }

            var length = new System.IO.FileInfo(args[0]).Length;
            var chunks = length / chunkSize;
            if(length % chunkSize != 0)
            {
                chunks++;
            }

            var parts = new List<string>();

            var fileNameOnly = Path.GetFileName(args[0]);
            using (StreamReader sr = new StreamReader(args[0]))
            {
                var chunk = new char[chunkSize];
                int cnc = CalcCnc(args[0]);

                for (int i = 1; i <= chunks; i++)
                {
                    var bytesRead = sr.ReadBlock(chunk, 0, chunkSize);

                    var chunkName = args[0] + "_part" + i;
                    parts.Add(Path.GetFileName(chunkName));
                    using (var writer = new StreamWriter(chunkName))
                    {
                        var header = "";
                        if(i == 1)
                        {
                            header += "b ";
                        }
                        else if (i == chunks)
                        {
                            header += "e ";
                        }
                        else
                        {
                            header += "p ";
                        }

                        header += fileNameOnly;
                        header += " ";                       
                        header += cnc;
                        
                        writer.Write(header);
                        writer.WriteLine();

                        writer.Write(chunk, 0, bytesRead);
                    }
                }
            }

            foreach(var c in parts)
            {
                Console.Write(c);
                Console.WriteLine();
            }            
        }

        static int CalcCnc(string fileName)
        {
            int cnc = 0;
            using (StreamReader sr = new StreamReader(fileName))
            {
                while(!sr.EndOfStream)
                {
                    var c = (int)sr.Read();
                    cnc += c;
                }
            }

            return cnc % 65535;
        }
    }
}
