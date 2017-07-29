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
        private int nPackageType;
        private long nPackageTime;
        private int nUserId;
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

        //这里发送float变量
        public void SendMessageToServer(float obj)
        {
            SocketManager.GetSingleton().GetSocket().Send(BitConverter.GetBytes(obj));
        }

        //这里向服务器发送Vector3变量，在这次Demo中这里是本地玩家的坐标信息
        public void SendMessageToServer(Vector3 obj)
        {
            byte[] Temp = new byte[100];
            //包头：类型 | 时间戳 | 发送目标
            //类型是与服务器约定好的
            Temp[0] = BitConverter.GetBytes(1);
            Temp[sizeof(int)] = BitConverter.GetBytes((long)13432);
            Temp[sizeof(int)+sizeof(long)] = BitConverter.GetBytes(2);
            //包的内容

            Temp[sizeof(int) + sizeof(long)+sizeof(int)] = BitConverter.GetBytes(obj.x)[0];
            Temp[sizeof(int) + sizeof(long) + 2*sizeof(int)] = BitConverter.GetBytes(obj.y)[0];
            Temp[sizeof(int) + sizeof(long) + 3 * sizeof(int)] = BitConverter.GetBytes(obj.z)[0];
            int res=SocketManager.GetSingleton().GetSocket().Send(Temp);
        }


        private void recvFromServer()
        {
            SocketManager.GetSingleton().GetSocket().Receive(recv);
        }
     
    
        //待完善---这里写接收信息的部分，注意线程的管理和内存的管理
        
        public void  RecvMessageFromServer()
        {
            Thread thread = new Thread(recvFromServer);
       
            //解析包头
            this.nPackageType = BitConverter.ToInt32(recv, 0);
            this.nPackageTime = BitConverter.ToInt64(recv,sizeof(int));
            this.nUserId = BitConverter.ToInt32(recv, sizeof(int)+sizeof(long));
        }

        //包头：类型 | 时间戳 | 发送目标
        //待完善，这里负责解析信息中的坐标信息--志彬
        //已实现解析包中坐标信息的功能 ---永康
        public Vector3 GetTransform()
        {
            int nHeaderLen = sizeof(int)+sizeof(int)+sizeof(long);
            float fX = (float)BitConverter.ToDouble(recv, nHeaderLen);
            float fY = (float)BitConverter.ToDouble(recv, nHeaderLen+sizeof(double));
            float fZ = (float)BitConverter.ToDouble(recv, nHeaderLen+2*sizeof(double));
            return new Vector3(fX, fY, fZ);
        }


    }
}
