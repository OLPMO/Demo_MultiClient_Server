using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts.Network
{
    //这是一个工具类，用于想数据包中添加对应的数据类型
    static class PackageTool
    {
        private static int offset = DataPackage.GetSizeOfDataHead();

        //由于BitConver。GetBytes()函数无法接收模版参数，这里选择全部不使用模版   
        public static void AddBaseType(ref byte[] packge, int data)
        {
            for (int i = 0; i < sizeof(int); i += 1)
            {
                packge[offset + i] = BitConverter.GetBytes(data)[i];
            }

            offset += sizeof(int);
        }

        public static void AddBaseType(ref byte[] packge, float data)
        {
            for (int i = 0; i < sizeof(float); i += 1)
            {
                packge[offset + i] = BitConverter.GetBytes(data)[i];
            }

            offset += sizeof(float);
        }

        public static void AddBaseType(ref byte[] packge, double data)
        {
            for (int i = 0; i < sizeof(double); i += 1)
            {
                packge[offset + i] = BitConverter.GetBytes(data)[i];
            }

            offset += sizeof(double);
        }

        public static void AddBaseType(ref byte[] packge, long data)
        {
            for (int i = 0; i < sizeof(long); i += 1)
            {
                packge[offset + i] = BitConverter.GetBytes(data)[i];
            }

            offset += sizeof(long);
        }

        public static void AddBaseType(ref byte[] packge, char data)
        {
            for (int i = 0; i < sizeof(char); i += 1)
            {
                packge[offset + i] = BitConverter.GetBytes(data)[i];
            }

            offset += sizeof(char);
        }

        public static void AddBaseType(ref byte[] packge, bool data)
        {
            for (int i = 0; i < sizeof(bool); i += 1)
            {
                packge[offset + i] = BitConverter.GetBytes(data)[i];
            }

            offset += sizeof(bool);
        }

        public static void AddVector(ref byte[] packge, Vector2 data)
        {
            PackageTool.AddBaseType(ref packge, data.x);
            PackageTool.AddBaseType(ref packge, data.y);
        }

        public static void AddVector(ref byte[] packge, Vector3 data)
        {
            PackageTool.AddBaseType(ref packge, data.x);
            PackageTool.AddBaseType(ref packge, data.y);
            PackageTool.AddBaseType(ref packge, data.z);
        }

        public static void AddVector(ref byte[] packge, Vector4 data)
        {
            PackageTool.AddBaseType(ref packge, data.x);
            PackageTool.AddBaseType(ref packge, data.y);
            PackageTool.AddBaseType(ref packge, data.z);
            PackageTool.AddBaseType(ref packge, data.w);
        }

        //这个函数用于重置字节数组中添加数据的偏移量，即重置为数据包头的大小
        public static void AddReset()
        {
            PackageTool.offset = DataPackage.GetSizeOfDataHead();
        }

    }
}
