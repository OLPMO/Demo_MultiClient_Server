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
            int nPakcageType;
            long ldTime;
            int nUid;
        }
        private DataHead dhPackageHead;
        private static int nSizeOfDataHead;  //数据包的头

        private byte[] Data; //这里是用于发送的数据，包含了用户数据以及数据包的头

        public DataPackage(int nUid,int nPackageSize)
        {

            this.nSizeOfDataHead = sizeof(DataHead);
            Data = new byte[nPackageSize];
  
            this.dhPackageHead.nUid = nUid;
        }

        public static int GetSizeOfDataHead()  {
            return sizeof(int)*2+sizeof(long);
        }

        public byte[] GetDataRef()
        {
            return Data;
        }

        #region 这部分负责想发送的数据中添加用户数据，但是内部调用的是数据类型转换为byte型的工具函数
        private DataPackage AddDataHead()
        {
            /*获取系统当前时间的毫秒时间戳，不精确
             *  TimeSpan ts = DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, 0);
             *  this.ldTime = Convert.ToInt64(ts.TotalMilliseconds);
            */
            this.dhPackageHead.ldTime = GlobalTimeController.sharedInstance().GetTimeSync();
            Array.Copy(BitConverter.GetBytes(this.dhPackageHead.nPackageType),0,this.Data,0,sizeof(int));
            Array.Copy(BitConverter.GetBytes(this.dhPackageHead.ldTime), 0, this.Data, sizeof(int), sizeof(long));
            Array.Copy(BitConverter.GetBytes(this.dhPackageHead.nUid), 0, this.Data, sizeof(int)+sizeof(long), sizeof(int));
            return this;
        }

        public DataPackage AddHealth(int nHp)
        {
           
            Array.Copy(BitConverter.GetBytes(nHp), 0, this.Data, this.nSizeOfDataHead, sizeof(int));
            return this;
        }

        public DataPackage AddPositon(Vector3 Pos)
        {
            this.dhPackageHead.nPakcageType = 0;
            this.AddDataHead();
            
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
            this.AddDataHead();
            strName += '\0';
            strPass += '\0';
            byte[] arrByteName = System.Text.Encoding.ASCII.GetBytes(strName);
            Array.Copy(arrByteName, 0, this.Data, this.nSizeOfDataHead, sizeof(arrByteName));
            byte[] arrBytePass = System.Text.Encoding.ASCII.GetBytes(strPass);
            Array.Copy(arrBytePass, 0, this.Data, this.nSizeOfDataHead + sizeof(arrByteName), sizeof(arrBytePass));
            return this;
        }
        #endregion


    }
}
