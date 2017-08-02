using System.Collections;
using System.Collections.Generic;
using UnityEngine;


// 服务器数据包队列
public class ServPackQueue
{
    Queue<ServDataPacket> queue = new Queue<ServDataPacket>();

    private static ServPackQueue instance = new ServPackQueue();


    public static ServPackQueue sharedInstance()
    {
        return instance;
    }


    // 数据包入队
    public void Push(ref ServDataPacket pack)
    {
        queue.Enqueue(pack);
    }


    // 数据包出队
    public ServDataPacket Pop()
    {
        if (queue.Count == 0) return null;
        return queue.Dequeue();
    }


    // 数据个数
    public int Size()
    {
        return queue.Count;
    }


    // 是否为空
    public bool IsEmpty()
    {
        return (queue.Count == 0);
    }


    // 清空数据
    public void Clear()
    {
        queue.Clear();
    }


}
