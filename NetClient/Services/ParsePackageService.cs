using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
namespace NetParsePackageService
{
    class ParsePackageService
    {
        private static void GetPosition(ref byte[] btRecv,ref PackageMetaData pmdData) {
            double lfX = BitConverter.ToDouble(btRecv, pmdData.nHeaderLen);
            double lfY = BitConverter.ToDouble(btRecv, pmdData.nHeaderLen + sizeof(double));
            double lfZ = BitConverter.ToDouble(btRecv, pmdData.nHeaderLen + 2*sizeof(double));
            pmdData.Set("x",lfX.ToString());
            pmdData.Set("y",lfY.ToString());
            pmdData.Set("z",lfZ.ToString());
        }
        /*解析登录信息的，手贱+傻逼，不小心写多
		private static void GetLoginInfo(ref byte[] btRecv,ref PackageMetaData pmdData){
            string strUserName="",strUserPass="";
            int nNameLen = 0;
            for (int i=pmdData.nHeaderLen;btRecv[i]!='\0';i++) {
                byte[] arrTmpByte = new byte[] { btRecv[i] };
                strUserName += System.Text.Encoding.ASCII.GetString(arrTmpByte);
                nNameLen++;
            }
            for (int i=(pmdData.nHeaderLen+nNameLen+1); btRecv[i] != '\0';i++) {
                byte[] arrTmpByte = new byte[] { btRecv[i] };
                strUserPass += System.Text.Encoding.ASCII.GetString(arrTmpByte);
            }
            pmdData.Set("name",strUserName);
            pmdData.Set("password", strUserPass);
        }*/
        //包内容编号（int）|时间戳（long）|玩家id（int）
        private static PackageMetaData ParseHeader(byte[] btRecv) {
            PackageMetaData data=new PackageMetaData();
            data.Set("type",BitConverter.ToInt32(btRecv, 0).ToString());
            data.Set("time", BitConverter.ToInt64(btRecv, sizeof(System.Int32)).ToString());
            data.Set("id", BitConverter.ToInt32(btRecv, sizeof(System.Int32)+sizeof(System.Int64)).ToString());
            return data;
        }
        public static PackageMetaData Parse(byte[] btRecv) {
            PackageMetaData pdata = ParseHeader(btRecv);
            int nHeaderLen = sizeof(System.Int32) * 2 + sizeof(System.Int64);
            int nPackageType = int.Parse(pdata.Get("type"));
            //Console.WriteLine("type int:" +nPackageType.ToString());
            pdata.nHeaderLen = nHeaderLen;
            //包类型编号小于0的没有内容
            if (nPackageType >= 0) {
                switch (nPackageType) {
                    case 0:
                        GetPosition(ref btRecv, ref pdata);
                        break;
                   
                }
            }
            return pdata;
        }
        /*测试代码
        static void Main(string[] args)
        {
            byte[] recv = new byte[100];
            Array.Copy(BitConverter.GetBytes(1),0,recv,0,sizeof(System.Int32));
            Array.Copy(BitConverter.GetBytes((System.Int64)764589), 0, recv, sizeof(System.Int32), sizeof(System.Int64));
            Array.Copy(BitConverter.GetBytes(3), 0, recv, sizeof(System.Int32) + sizeof(System.Int64), sizeof(System.Int32));
            //测试位置信息的读取
              Array.Copy(BitConverter.GetBytes((double)2.0523), 0, recv, 2*sizeof(System.Int32) + sizeof(System.Int64), sizeof(double));
            Array.Copy(BitConverter.GetBytes((double)3.2326), 0, recv, 2*sizeof(System.Int32) + sizeof(double)+sizeof(System.Int64), sizeof(double));
            Array.Copy(BitConverter.GetBytes((double)62.5465), 0, recv, 2*sizeof(System.Int32) + 2*sizeof(double)+sizeof(System.Int64), sizeof(double));
           
            Array.Copy(System.Text.Encoding.ASCII.GetBytes("aaa\0"), 0, recv, 2 * sizeof(System.Int32) + sizeof(System.Int64), System.Text.Encoding.ASCII.GetBytes("aaa\0").Length);
            Array.Copy(System.Text.Encoding.ASCII.GetBytes("123456\0"), 0, recv, 2 * sizeof(System.Int32) + sizeof(System.Int64)+ System.Text.Encoding.ASCII.GetBytes("aaa\0").Length, System.Text.Encoding.ASCII.GetBytes("123456\0").Length);
            PackageMetaData test = Parse(recv);

            //Console.WriteLine(test.Get("x"));
            //Console.WriteLine(test.Get("y"));
            //Console.WriteLine(test.Get("z"));
            Console.WriteLine(test.Get("name"));
        }*/
    }
}
