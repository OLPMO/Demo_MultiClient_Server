using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using NetParsePackageService;
using System.Threading;

namespace Assets.Scripts.Network
{
    class ReceiveData
    {
        private static ReceiveData RecvDataInstance = null;
        private static readonly System.Object LockHelper = new System.Object();
       // private static readonly System.Object LockDataHelper = new System.Object();
        private static byte[] RecvData;
       // private PackageMetaData RecvPackage;

       // private static Queue<PackageMetaData> RecvDataQueue;  //这里是一个用于存放某一个玩家数据的队列，限定最多存放5个数据包
        private int nQueueMaxLen;
        private Dictionary<String, Queue<PackageMetaData>> RecvDataPool; //这里是一个用于存放所有玩家数据的数据池，里面根据数据包的类型创建队列
        private bool firstTimeRecvOtherPlayer;
        private int nQuitTimes;
        private static int nOtherID;

        private ReceiveData()
        {
            RecvData = new byte[80];
            Debug.Log("Here we create a byte array to receive data from server! The size is 80 byte!");

            //这里限定了队列的长度为5
            //RecvDataQueue = new Queue<PackageMetaData>();
            nQueueMaxLen = 5;
            RecvDataPool = new Dictionary<String, Queue<PackageMetaData>>();
            firstTimeRecvOtherPlayer = false;
            nQuitTimes = 0;

            Thread recvThread = new Thread(RecvAndParseData);
            recvThread.IsBackground = true;
            Debug.Log("Here we start a thread to receive package!");
            recvThread.Start();
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
            while (true)
            {
                if (nQuitTimes >= 999)
                {
                    Debug.Log("Please check your internet,you might disconnected! The QuitTimes is :"+nQuitTimes+" now!");
                    return;
                }
               
                if (NetworkManager.GetSingleton().RecvPackFromServer(ref RecvData) <= 0)
                {
                    nQuitTimes += 1;
                    Thread.Sleep(5);
                    Debug.Log("Here the nQuitTimes is : " + nQuitTimes+"When it comes to 999,end the thread!");
                    continue;
                }
                nQuitTimes = 0;

                Debug.Log("Here we receive a package from server and parse it!");
                PackageMetaData RecvPackage=RecvPackage = ParsePackageService.Parse(RecvData);

                //这里获取一下包的类型，以便于选择是否创建新的队列
                var PackageType=RecvPackage.Get("Type");
                if (! RecvDataPool.ContainsKey(PackageType))
                {
                    var TypeQueue = new Queue<PackageMetaData>();
                    RecvDataPool.Add(PackageType, TypeQueue);
                    Debug.Log("Here we create a new queue!");
                }

                if (firstTimeRecvOtherPlayer == false)
                {
                    nOtherID = Convert.ToInt32(RecvPackage.Get("ID"));
                    if (nOtherID > 0)
                    {
                        firstTimeRecvOtherPlayer = true;
                    }
                }

                if (RecvDataPool[PackageType].Count > nQueueMaxLen)
                {
                    Debug.Log("Too many packages in the queue, let's delete the front one!");
                    RecvDataPool[PackageType].Dequeue();
                }
                Debug.Log("Here we add a parsed package to the queue!");
                RecvDataPool[PackageType].Enqueue(RecvPackage);
            }
        }

        public PackageMetaData GetLatestPackage(String Type)
        {
            Debug.Log("Here we are trying to return the front package and delete it from queue!");

            if(!RecvDataPool.ContainsKey(Type))
            {
                Debug.Log("RecvDataPool isn't have data in type of " + Type);
                return null;
            }

            if(RecvDataPool[Type].Count>=0)
            {
                Debug.Log("Here we have data in " + Type + "Queue!");
                return RecvDataPool[Type].Dequeue();
            }
            return null;
        }
    }
}
