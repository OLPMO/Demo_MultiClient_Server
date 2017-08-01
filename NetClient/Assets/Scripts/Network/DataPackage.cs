using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using Assets.Scripts.Network;

namespace Assets.Scripts
{
    class DataPackage: DataInterface
    {
        private struct DataHead
        {
            //这个由永康负责编写，这里保存了数据包的头的结构
            int a;
        }
        private static int SizeOfDataHead;  //数据包的头

        private byte[] Data; //这里是用于发送的数据，包含了用户数据以及数据包的头

        public DataPackage(int nPackageSize)
        {
            Data = new byte[nPackageSize];     
            PackageTool.AddReset();
        }

        public static int GetSizeOfDataHead()  
        {
            //这个待定，因为数据包的头需要永康完善
            return 0;
        }

        public byte[] GetDataRef()
        {
            return Data;
        }

        #region 这部分负责想发送的数据中添加用户数据，但是内部调用的是数据类型转换为byte型的工具函数
        public DataPackage AddDataHead(int a)
        {
            //这部分永康完成
            //永康需要
            return this;
        }

        public DataPackage AddHealth(int hud)
        {
            PackageTool.AddBaseType(ref Data,hud);
            
            return this;
        }

        public DataPackage AddPositon(Vector3 Pos)
        {
            PackageTool.AddVector(ref Data, Pos);
            return this;
        }

        public void PackPlayerUp()  //这些打包函数由永康完成
        {
            
            //这里负责添加包的内容

            //eg，先添加数据包头，然后在数据包中依次添加后面的数据
            AddDataHead(1).AddHealth(2).AddPositon(new Vector3(0,0,0));

        } 
        #endregion


    }
}
