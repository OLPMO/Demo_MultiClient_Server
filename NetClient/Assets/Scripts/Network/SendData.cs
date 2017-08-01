using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts.Network
{
    class SendData
    {
       // private static readonly System.Object LockHelper = new System.Object();
        // private static readonly System.Object LockDataHelper = new System.Object();
        private DataPackage SendPackage;

        //这里初始化一个SendData实例，并在内部创建一个用于发送的数据包
        public SendData(int nPackageSize)
        {
            SendPackage = new DataPackage(nPackageSize);
        }

        
        public DataPackage GetDataPackage()
        {
            return SendPackage;
        }

        public int Send()
        {
            return NetworkManager.GetSingleton().SendPackToServer(SendPackage);
        }

        //public static SendData GetSingleton()
        //{
        //    if (SendDataInstance == null)
        //    {
        //        lock (LockHelper)
        //        {
        //            if (SendDataInstance == null)
        //            {
        //                SendDataInstance = new SendData();
        //                Debug.Log("Here we create a RecvDataInstance to handle received data from server!");
        //            }
        //        }
        //    }
        //    return SendDataInstance;
        //}
    }
}
