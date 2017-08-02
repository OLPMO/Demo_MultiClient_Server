using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
using System.Collections.Generic;
namespace NetParsePackageService
{
    class PackageMetaData{
        private Dictionary<string, string> dicData = new Dictionary<string, string>();
        public int nHeaderLen;
        public string Get(string strKey){
            return dicData[strKey];
        }
        public void Set(string strKey,string strVal) {
       
            dicData[strKey] = strVal;
        }
    }
}
