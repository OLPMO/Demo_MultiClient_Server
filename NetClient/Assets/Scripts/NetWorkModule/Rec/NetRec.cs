using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Assets.Scripts.Player;

public class NetRec
{
    private static NetRec netRecInstance = null;
    private Socket RecSocket;

    public static NetRec GetSingleton()
    {
        if (netRecInstance == null)
            netRecInstance = new NetRec();

        return netRecInstance;
    }

    private NetRec()
    {
        RecSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        //服务器信息
        IPAddress ipServerAddress = IPAddress.Parse("10.20.73.61");
        IPEndPoint ipServerEndPoint = new IPEndPoint(ipServerAddress, 10000);
        IAsyncResult Result = RecSocket.BeginConnect(ipServerEndPoint, new AsyncCallback(connectCal), RecSocket);

        bool success = Result.AsyncWaitHandle.WaitOne(5000, true);
        if (!success)
        {
            Closed();
            Debug.Log("超时");
        }
    }

    public Vector3 ReceiveMessage()
    {
        //这里接受并解析出坐标信息
        //RecSocket.Receive()
        //返回坐标信息     

        return new Vector3(0, 0, 0);                                                                                                                                                                                        
    }



    private void connectCal(IAsyncResult asyncConnect)
    {
        Debug.Log("connectSuccess");
    }

    //关闭Socket     
    public void Closed()
    {

        if (RecSocket != null && RecSocket.Connected)
        {
            RecSocket.Shutdown(SocketShutdown.Both);
            RecSocket.Close();
        }
        RecSocket = null;
    }


    public Vector3 RecTransform()
    {
        return new Vector3(0,0,0);
    }


}
