using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;


// 数据包目标
public enum PACK_TO
{
    BOARDCASR = -1, // 目标 - 广播发送
    SERVER = -2,    // 目标 - 服务器处理
}


// 数据包来源
public enum PACK_FROM
{
    SERVER = -1, // 来源 - 服务器主动
}


// 数据包类型
public enum PACK_TYPE
{
    LOGIN = -1,  // 请求 - 登录验证
    ERROR = -2,  // 反馈 - 发生错误
    DENY = -3,   // 反馈 - 拒绝登录
    ACCEPT = -4, // 反馈 - 登录成功
}


public class ServerConnecter
{

    private static ServerConnecter instance = new ServerConnecter();

    private Socket socket = null;


    // 获取全局单例
    public static ServerConnecter sharedInstance()
    {
        return instance;
    }


    // 建立服务器连接 - ip地址 / 端口
    public void Connect(string host, int port, AsyncCallback succCallback)
    {
        if (socket != null) Close();
        socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        socket.BeginConnect(host, port, succCallback, socket);
    }


    // 是否已连接到服务器
    public bool IsConnected()
    {
        return socket.Connected;
    }


    // 登录 - 用户名 / 密码 / 连接成功回调
    public void Login(string name, string pwd)
    {
        // 发送登录数据
        ServDataPacket pack = ServPackParser.MakeLoginPacket(name, pwd);
        Send(ref pack);

        // 开启接收数据线程
        Thread thread = new Thread(new ThreadStart(Receive));
        thread.IsBackground = true;
        thread.Start();

    }


    // 关闭连接
    public void Close()
    {
        if (socket == null || socket.Connected == false) return;
        socket.Shutdown(SocketShutdown.Both);
        socket.Close();
        socket = null;
    }


    // 发送数据 - 直接发送字节数组 - 字节数据
    public void Send(ref ServDataPacket pack)
    {
        if (socket.Connected == false) return;
        IAsyncResult asyncSend = socket.BeginSend(pack.data, 0, pack.data.Length, SocketFlags.None, null, socket);
    }
    

    // 接收数据线程
    private void Receive()
    {
        if (IsConnected() == false) return;

        ServPackQueue quePacket = ServPackQueue.sharedInstance();
        while(true)
        {
            ServDataPacket pack = new ServDataPacket();
            int len = socket.Receive(pack.data, ServDataPacket.LENGTH, SocketFlags.None);
            if (len <= 0)
            {
                // TODO 连接断开处理

                break;
            }

            quePacket.Push(ref pack); // 将接收到的消息插入到队列中

        }
    }


    


    

}
