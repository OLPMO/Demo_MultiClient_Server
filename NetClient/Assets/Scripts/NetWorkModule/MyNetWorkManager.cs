using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.Networking;
using Assets.Scripts.Player;
using System.Net;
using System.Net.Sockets;
using UnityEngine;
using System.Collections;
using System.Threading;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace Assets.Scripts.NetWorkModule
{
    class MyNetWorkManager : NetworkBehaviour
    {
        private Socket clientSocket;
        private Dictionary<int, Role> PlayerMap;
        private static MyNetWorkManager myNetWorkManagerInstance = null;


        public static MyNetWorkManager GetSinglton()
        {
            if (myNetWorkManagerInstance == null)
                myNetWorkManagerInstance = new MyNetWorkManager();

            return myNetWorkManagerInstance;
        }

        private MyNetWorkManager()
        {


        }
    }
}
