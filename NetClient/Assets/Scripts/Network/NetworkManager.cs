using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

namespace Assets.Scripts
{
    class NetworkManager
    {
        private Socket ClientSocket = null;
        private static NetworkManager NMInstance = null;  //这是网络管理器的实例
        private static readonly System.Object LockHelper = new System.Object();  //用于创建实例时的加锁变量
        private static readonly System.Object LockSocketHelper = new System.Object();  //用于重新创建Socket时的加锁变量
        private float fReConnectTime;

        //服务器IP和端口号
        private String ServerIP { set; get; }
        private int ServerPort { set; get; }


        public static NetworkManager GetSingleton()
        {
            if (NMInstance == null)
            {
                lock (LockHelper)
                {
                    if (NMInstance == null)
                    {
                        NMInstance = new NetworkManager();
                        Debug.Log("Here we creat a NetworkManager Instance !");
                    }
                }
            }
            return NMInstance;
        }

        private NetworkManager()
        {
            //这里需要添加设置服务器IP和端口号的代码


            ConToServer("",1234, ConSucced);
            Debug.Log("Now ,we can communicate with server !");
            TryReConnect();
        }

        #region 这里负责网络连接、包括socket的创建，与服务器的连接操作，断线重连操作等
        private bool CreateSocketTcp()
        {
            try
            {
                if (ClientSocket != null)
                    return true;
                lock (LockSocketHelper)
                {
                    if (ClientSocket != null)
                        return true;
                    Debug.Log("Here we create a socket!");
                    ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                }
            }
            catch (Exception ex)
            {
                Debug.Log("Create socket failed ! Error message: "+ex.Message);
                CloseSocket();
                return false;
            }

            Debug.Log("Creating socket is successed !");
            return true;
        }

        private void CloseSocket()
        {
            if (ClientSocket != null && ClientSocket.Connected==true)
            {
                ClientSocket.Shutdown(SocketShutdown.Both);
                ClientSocket.Close();
                ClientSocket = null;
                Debug.Log("Socket is closed now !");
            }
        }

        private bool ConToServer(String ServerAddress,int Port, AsyncCallback ConCallback)
        {
            while (ClientSocket == null)
                CreateSocketTcp();

            IPAddress address = IPAddress.Parse(ServerAddress);
            try
            {
                ClientSocket.BeginConnect(address, Port, new AsyncCallback(ConCallback), ClientSocket);
            }
            catch (Exception ex)
            {
                Debug.Log("Connecting failed! Error message is: " + ex.Message);
                return false;
            }

            return true;
        }

        private  void ConSucced(IAsyncResult res)
        {
            Debug.Log("Connecting successed!");
        }

        //这里用于处理断线重连
        private void TryReConnect()
        {
            Thread ReconThread = new Thread(ReConnect);
            ReconThread.IsBackground = true;
            Debug.Log("Here is a new thread for disconnect check!");
            ReconThread.Start();
        }

        //public String GetServerIP() {return ServerIP;}
        //public int GetServerPort() { return ServerPort; }
        //public void SetServerIP(String Ip) { ServerIP = Ip; }
        //public void SetServerPort(int Port) { ServerPort = Port; }

        private void ReConnect()
        {
            while (true)
            {
                try
                {
                    //这里用于判断是否掉线
                    Debug.Log("Here we testing whether disconnected or not!");
                    int sendNum = ClientSocket.Send(Encoding.ASCII.GetBytes("test connected"),SocketFlags.None);
                    if (sendNum >= Encoding.ASCII.GetByteCount("test connected"))
                    {
                        Debug.Log("We still connected! Let's sleep 5 second until another check!");
                        Thread.Sleep(5000);
                        Debug.Log("Start another check!");
                    }
                }
                catch (Exception ex)
                {
                    //这里用于重连
                    Debug.Log("Disconnected error message is: " + ex.Message);
                    Debug.Log("We disconnected! We should Reconnect!");

                    if (ClientSocket == null)
                    {
                        Debug.Log("Socket is invalid, we should recreate a socket!");
                        while (ClientSocket == null)
                            CreateSocketTcp();
                    }
                    ConToServer(ServerIP, ServerPort, ConSucced);
                    Debug.Log("Here we reconnected! ");   
                }
            }
        }
        #endregion

        #region 这里负责数据的发送操作
        public int SendPackToServer(DataPackage data)
        {
            try
            {
                int nSendNum = ClientSocket.Send(data.GetDataRef());
                Debug.Log("Sengding data was successed, and we have send " + nSendNum + "Byte !");
                return nSendNum;
            }
            catch (Exception ex)
            {
                Debug.Log("Sending data failed! Error message was : "+ex.Message);
                return -1;
            }
        }

        public int RecvPackFromServer(ref byte[] data)
        {
            try
            {
                int nRecvNum=ClientSocket.Receive(data);
                Debug.Log("Receiving data was successed, and we have received " + nRecvNum + "Byte !");
                return nRecvNum;
            }
            catch (Exception ex)
            {
                Debug.Log("Receiving data failed ! Error message was: " + ex.Message);
                return -1;
            }
        }

        #endregion
    }
}
