using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


// 服务器数据包
public class ServDataPacket
{
    public static int LENGTH = 80;
    public byte[] data = new byte[LENGTH];
}


// 服务器数据包解析器
public class ServPackParser
{

    // 封装 - 登录验证数据包 - 时间戳 / 用户名 / 密码
    public static ServDataPacket MakeLoginPacket(string name, string pwd)
    {
        ServDataPacket pack = new ServDataPacket();
        AddPacketHead(ref pack, (int)PACK_TYPE.LOGIN, 0, 0);
        byte[] bytesName = System.Text.Encoding.ASCII.GetBytes(name);
        byte[] bytesPwd = System.Text.Encoding.ASCII.GetBytes(pwd);
        for (int i = 0; i < 32; i++)
        {
            pack.data[i + 16] = (i < bytesName.Length ? bytesName[i] : (byte)0);
            pack.data[i + 48] = (i < bytesPwd.Length ? bytesPwd[i] : (byte)0);
        }

        return pack;
    }

    
    // 封装 - 带有一个数据信息的包 - 类别标识 / 时间戳 / 目标ID / 参数
    public static ServDataPacket MakePacket<T>(Int32 type, long milli, Int32 toID, T parm)
    {
        ServDataPacket pack = new ServDataPacket();
        AddPacketHead(ref pack, type, milli, toID);
        AddBytes<T>(ref pack, parm, 16);

        return pack;
    }


    // 封装 - 带有两个数据信息的包 - 类别标识 / 时间戳 / 目标ID / 参数
    public static ServDataPacket MakePacket<T1, T2>(Int32 type, long milli, Int32 toID, T1 parm1, T2 parm2)
    {
        ServDataPacket pack = new ServDataPacket();
        AddPacketHead(ref pack, type, milli, toID);

        int pos = 16;
        pos += AddBytes<T1>(ref pack, parm1, pos);
        AddBytes<T2>(ref pack, parm2, pos);

        return pack;
    }


    // 封装 - 带有三个数据信息的包 - 类别标识 / 时间戳 / 目标ID / 参数
    public static ServDataPacket MakePacket<T1, T2, T3>(Int32 type, long milli, Int32 toID, T1 parm1, T2 parm2, T3 parm3)
    {
        ServDataPacket pack = new ServDataPacket();
        AddPacketHead(ref pack, type, milli, toID);

        int pos = 16;
        pos += AddBytes<T1>(ref pack, parm1, pos);
        pos += AddBytes<T2>(ref pack, parm2, pos);
        AddBytes<T3>(ref pack, parm3, pos);

        return pack;
    }


    // 解析 - 类型
    public static int ParsePacketType(ref ServDataPacket pack)
    {
        return BitConverter.ToInt32(pack.data, 0);
    }


    // 解析 - 时间戳
    public static long ParsePacketTimeStamp(ref ServDataPacket pack)
    {
        return BitConverter.ToInt64(pack.data, 4);
    }


    // 解析 - 来源ID
    public static int ParsePacketFromID(ref ServDataPacket pack)
    {
        return BitConverter.ToInt32(pack.data, 12);
    }


    // 解析 - 数据头 - 类型 / 时间戳 / 来源ID
    public static void ParsePacketHead(ref ServDataPacket pack, out Int32 type, out long milli, out Int32 fromID)
    {
        type = ParsePacketType(ref pack);
        milli = ParsePacketTimeStamp(ref pack);
        fromID = ParsePacketFromID(ref pack);
    }


    // 解析带有一个数据信息的包 - 参数返回
    public static void ParsePacket<T>(ref ServDataPacket pack, out T data)
    {
        GetData<T>(ref pack, out data, 16);
    }


    // 解析带有两个数据信息的包 - 参数返回
    public static void ParsePacket<T1, T2>(ref ServDataPacket pack, out T1 data1, out T2 data2)
    {
        int pos = 16;
        pos += GetData<T1>(ref pack, out data1, pos);
        GetData<T2>(ref pack, out data2, pos);
    }


    // 解析带有三个数据信息的包 - 参数返回
    public static void ParsePacket<T1, T2, T3>(ref ServDataPacket pack, out T1 data1, out T2 data2, out T3 data3)
    {
        int pos = 16;
        pos += GetData<T1>(ref pack, out data1, pos);
        pos += GetData<T2>(ref pack, out data2, pos);
        GetData<T3>(ref pack, out data3, pos);
    }


    // 添加数据包头
    private static void AddPacketHead(ref ServDataPacket pack, Int32 type, long milli, Int32 toID)
    {
        AddBytes<int>(ref pack, type, 0);
        AddBytes<long>(ref pack, milli, 4);
        AddBytes<int>(ref pack, toID, 12);
    }


    // 将一个类型的数据转换为字节码并添加到bytes数组位置 - 返回添加的字节数
    // 支持类型 - int / long / unit / ulong / double / bool
    private static int AddBytes<T>(ref ServDataPacket pack, T val, int pos)
    {
        int len = 0;
        Type type = typeof(T);
        byte[] tmpBytes = null;
        if (type == typeof(Int32)) // 整型
        {
            len = 4;
            tmpBytes = BitConverter.GetBytes(Convert.ToInt32(val));
        }
        else if (type == typeof(Int64)) // 长整型
        {
            len = 8;
            tmpBytes = BitConverter.GetBytes(Convert.ToInt64(val));
        }
        else if (type == typeof(UInt32)) // 无符号整型
        {
            len = 4;
            tmpBytes = BitConverter.GetBytes(Convert.ToUInt32(val));
        }
        else if (type == typeof(UInt64)) // 无符号长整形
        {
            len = 8;
            tmpBytes = BitConverter.GetBytes(Convert.ToUInt64(val));
        }
        else if(type == typeof(float)) // 浮点型
        {
            len = 4;
            tmpBytes = BitConverter.GetBytes(Convert.ToDouble(val));
        }
        else if (type == typeof(double)) // 双精度浮点
        {
            len = 4;
            tmpBytes = BitConverter.GetBytes(Convert.ToSingle(val));
        }
        else if (type == typeof(bool)) // 布尔型
        {
            len = 4;
            tmpBytes = BitConverter.GetBytes(Convert.ToBoolean(val));
        }

        for (int i = 0; i < len; i++)
            pack.data[pos + i] = tmpBytes[i];

        return len;
    }

    
    // 解析 - 字节数据解析 - 返回转换使用的byte字节数
    private static int GetData<T>(ref ServDataPacket pack, out T val, int pos)
    {
        int len = 0;
        val = default(T);
        Type type = typeof(T);
        if(type == typeof(Int32))
        {
            len = 4;
            val = (T)Convert.ChangeType(BitConverter.ToInt32(pack.data, pos), type);
        }
        else if(type == typeof(Int64))
        {
            len = 8;
            val = (T)Convert.ChangeType(BitConverter.ToInt64(pack.data, pos), type);
        }
        else if(type == typeof(UInt32))
        {
            len = 4;
            val = (T)Convert.ChangeType(BitConverter.ToUInt32(pack.data, pos), type);
        }
        else if(type == typeof(UInt64))
        {
            len = 8;
            val = (T)Convert.ChangeType(BitConverter.ToUInt64(pack.data, pos), type);
        }
        else if(type == typeof(float))
        {
            len = 4;
            val = (T)Convert.ChangeType(BitConverter.ToSingle(pack.data, pos), type);
        }
        else if(type == typeof(double))
        {
            len = 8;
            val = (T)Convert.ChangeType(BitConverter.ToDouble(pack.data, pos), type);
        }
        else if(type == typeof(bool))
        {
            len = 4;
            val = (T)Convert.ChangeType(BitConverter.ToBoolean(pack.data, pos), type);
        }

        return len;
    }

}
