using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Assets.Scripts.Player;

public class NetSend :MonoBehaviour
{
    private static NetSend netSendInstance = null;
    private Socket SendSocket;
    public static NetSend GetSingleton()
    {
        if (netSendInstance == null)
            netSendInstance = new NetSend();

        return netSendInstance;
    }

    private NetSend()
    {
        SendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        //服务器信息
        IPAddress ipServerAddress = IPAddress.Parse("10.20.73.61");
        IPEndPoint ipServerEndPoint = new IPEndPoint(ipServerAddress, 10000);
        IAsyncResult Result = SendSocket.BeginConnect(ipServerEndPoint, new AsyncCallback(connectCal), SendSocket);

        bool success = Result.AsyncWaitHandle.WaitOne(5000, true);
        if (!success)
        {
            Closed();
            Debug.Log("超时");
        }
    }

    public void SendMessage(Vector3 LocalPlayerPosition)
    {
        if(SendSocket==null)
        {
            return;
        }
       
        
        //发送数据
        //------------------------------            ---------------------------------------
    }

    private void connectCal(IAsyncResult asyncConnect)
    {
        Debug.Log("connectSuccess");
    }

    //关闭Socket     
    public void Closed()
    {

        if (SendSocket != null && SendSocket.Connected)
        {
            SendSocket.Shutdown(SocketShutdown.Both);
            SendSocket.Close();
        }
        SendSocket = null;
    }

}
