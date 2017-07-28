using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using UnityEngine;

namespace Assets.Scripts.NetWorkModule
{
    class SocketManager
    {
        private Socket ClientSocket = null;
        private static SocketManager SocketManagerInstance;
        private static readonly System.Object LockHelper = new System.Object();
        private SocketManager()
        {
            ConnectToServer();
        }

        public static SocketManager GetSingleton()
        {

            if (SocketManagerInstance == null)
            {
                lock (LockHelper)
                {
                    if(SocketManagerInstance==null)
                    SocketManagerInstance = new SocketManager();
                }
            }
            return SocketManagerInstance;
        }

        public void ConnectToServer()
        {
            IPAddress IPServer = IPAddress.Parse("10.20.73.61");
            IPEndPoint PortServer = new IPEndPoint(IPServer, 7777);
            ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                ClientSocket.Connect(PortServer);
            }
            catch (Exception ex)
            {
                Debug.Log(ex.Message);
                SocketClose();
            }

            Debug.Log("Server Connected !");
        }

        public void SocketClose()
        {
            if (ClientSocket != null)
                ClientSocket.Close();
        }

        public Socket GetSocket()
        {
            return ClientSocket;
        }

    }
}
