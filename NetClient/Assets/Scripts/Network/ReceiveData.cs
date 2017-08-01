using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts.Network
{
    class ReceiveData
    {
        private static ReceiveData RecvDataInstance = null;
        private static readonly System.Object LockHelper = new System.Object();
       // private static readonly System.Object LockDataHelper = new System.Object();
        private static byte[] RecvData;
        private PackageData RecvPackage;
        private Queue<PackageData> RecvDataQueue;  //这里是一个用于存放某一个玩家数据的队列，限定最多存放5个数据包
        private Dictionary<int, Queue<PackageData>> RecvDataPool; //这里是一个用于存放所有玩家数据的数据池，里面根据玩家的标识符，这里假定为int型的数据，每个玩家有一个存放他的数据的队列；

        private ReceiveData()
        {
            RecvData = new byte[80];
            Debug.Log("Here we create a byte array to receive data from server! The size is 80 byte!");

            //这里限定了队列的长度为5
            //RecvDataQueue = new Queue<PackageData>(5);
            RecvDataPool = new Dictionary<int, Queue<PackageData>>();
        }

        public static ReceiveData GetSingleton()
        {
            if (RecvDataInstance == null)
            {
                lock (LockHelper)
                {
                    if(RecvDataInstance==null)
                    {
                        RecvDataInstance = new ReceiveData();
                        Debug.Log("Here we create a RecvDataInstance to handle received data from server!");
                    }
                }
            }
            return RecvDataInstance;
        }

        private void RecvAndParseData()
        {
            if(NetworkManager.GetSingleton().RecvPackFromServer(ref RecvData)<=0)
            {
                return;
            }
            Debug.Log("Here we receive a package from server and parse it!");
            RecvPackage = new PackageData(RecvData);
            
            int ID=RecvPackage.GetID();
        }

    }
}
