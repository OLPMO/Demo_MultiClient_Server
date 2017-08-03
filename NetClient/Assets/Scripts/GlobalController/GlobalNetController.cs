using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Assets.Scripts.Network;

namespace Assets.Scripts.GlobalController
{
    class GlobalNetController
    {
        public int sendCmd(int cmdCode,long currentTime)
        {
            SendData CmdData = new SendData(1000);
            CmdData.GetDataPackage();
            return 0;
        }
        
    }
}
