﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Volleyball
{
    class Logging
    {
        #region members
        String fileName;
        static readonly String logPath = "./log/";
        #endregion

        public Logging()
        {
            fileName = logPath + DateTime.Now.ToString("yyyyMMdd_HH_mm_ss") + "_log.txt";
        }
        
        public void write(String message)
        {
            using (StreamWriter sw = File.AppendText(fileName))
            {
                sw.WriteLine(DateTime.Now.ToString("yyyyMMdd_HH_mm_ss") + ": " + message);
            }
        }
    }
}
