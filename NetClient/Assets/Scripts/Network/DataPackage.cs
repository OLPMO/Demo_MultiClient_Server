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
        public struct DataHead
        {
            //这个由永康负责编写，这里保存了数据包的头的结构
            int nPakcageType;
            long ldTime;
            int nUid;
        }
        private DataHead dhPackageHead;
        private static int nSizeOfDataHead;  //数据包的头

        private byte[] Data; //这里是用于发送的数据，包含了用户数据以及数据包的头

        public DataPackage(int nUid,int nPackageSize)
        {

            SizeOfDataHead = sizeof(DataHead);
            Data = new byte[nPackageSize];
            //  PackageTool.AddReset();

            this.dhPackageHead.nUid = nUid;
        }

        public static int GetSizeOfDataHead()  
        {
            
            return sizeof(DataHead);
        }

        public byte[] GetDataRef()
        {
            return Data;
        }

        #region 这部分负责想发送的数据中添加用户数据，但是内部调用的是数据类型转换为byte型的工具函数
        private DataPackage AddDataHead(DataHead dhPackageHead)
        {
            TimeSpan ts = DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, 0);
            this.ldTime = Convert.ToInt64(ts.TotalMilliseconds);
            //这部分永康完成
            //永康需要
            //  Array.Copy(BitConverter.GetBytes((double)62.5465), 0, recv, 2*sizeof(System.Int32) + 2*sizeof(double)+sizeof(System.Int64), sizeof(double));
            Array.Copy(BitConverter.GetBytes(dhPackageHead.nPackageType),0,this.Data,0,sizeof(int));
            Array.Copy(BitConverter.GetBytes(dhPackageHead.ldTime), 0, this.Data, sizeof(int), sizeof(long));
            Array.Copy(BitConverter.GetBytes(dhPackageHead.nUid), 0, this.Data, sizeof(int)+sizeof(long), sizeof(int));
            return this;
        }

        public DataPackage AddHealth(int nHp)
        {
            //  PackageTool.AddBaseType(ref Data,hud);
            Array.Copy(BitConverter.GetBytes(nHp), 0, this.Data, this.nSizeOfDataHead, sizeof(int));
            return this;
        }

        public DataPackage AddPositon(Vector3 Pos)
        {
            this.dhPackageHead.nPakcageType = 0;
            this.AddDataHead(this.dhPackageHead);
            
           // PackageTool.AddVector(ref Data, Pos);
            float fX = (float)Pos.x;
            float fY = (float)Pos.y;
            float fZ = (float)Pos.z;
            byte[] arrByteX = BitConverter.GetBytes(fX);
            byte[] arrByteY = BitConverter.GetBytes(fY);
            byte[] arrByteZ = BitConverter.GetBytes(fZ);

            Array.Copy(arrByteX, 0, this.Data,this.nSizeOfData, sizeof(System.Single));
            Array.Copy(arrByteY, 0, this.Data, this.nSizeOfData + sizeof(System.Single), sizeof(System.Single));
            Array.Copy(arrByteZ, 0, this.Data, this.nSizeOfData + 2*sizeof(System.Single), sizeof(System.Single));
            return this;
        }
        public DataPackage AddLoginInfo(string strName,string strPass) {
            this.dhPackageHead.nPakcageType = -1;
            this.AddDataHead(this.dhPackageHead);
            strName += '\0';
            strPass += '\0';
            byte[] arrByteName = System.Text.Encoding.ASCII.GetBytes(strName);
            Array.Copy(arrByteName, 0, this.Data, this.nSizeOfDataHead, sizeof(arrByteName));
            byte[] arrBytePass = System.Text.Encoding.ASCII.GetBytes(strPass);
            Array.Copy(arrBytePass, 0, this.Data, this.nSizeOfDataHead + sizeof(arrByteName), sizeof(arrBytePass));
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
