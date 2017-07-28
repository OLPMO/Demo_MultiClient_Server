using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using System.Threading;

namespace Assets.Scripts.NetWorkModule
{
    class NetworkManager
    {
        private byte[] send = new byte[4096];
        private byte[] recv = new byte[4096];

        private static readonly System.Object LockHelper = new System.Object();
        private  static NetworkManager networkManagerInstance = null;

        private NetworkManager()
        {
            RecvMessageFromServer();
        }

        public static NetworkManager GetSingleton()
        {
            if (networkManagerInstance == null)
            {
                lock (LockHelper)
                {
                    if (networkManagerInstance == null)
                        networkManagerInstance = new NetworkManager();
                }
            }
            return networkManagerInstance;
        }




        public void SendMessageToServer(byte[] obj)
        {
            SocketManager.GetSingleton().GetSocket().Send(obj);
        }

        public void SendMessageToServer(float obj)
        {
            SocketManager.GetSingleton().GetSocket().Send(BitConverter.GetBytes(obj));
        }

        public void SendMessageToServer(Vector3 obj)
        {
            byte[] Temp = new byte[3];
            Temp[0] = BitConverter.GetBytes(obj.x)[0];
            Temp[1]= BitConverter.GetBytes(obj.y)[0];
            Temp[2]= BitConverter.GetBytes(obj.z)[0];
            int res=SocketManager.GetSingleton().GetSocket().Send(Temp);
        }

        private void recvFromServer()
        {
            SocketManager.GetSingleton().GetSocket().Receive(recv);
        }

        public void  RecvMessageFromServer()
        {
            Thread thread = new Thread(recvFromServer);
        }

        public Vector3 GetTransform()
        {
            return new Vector3(0, 0, 0);
        }

    }
}
