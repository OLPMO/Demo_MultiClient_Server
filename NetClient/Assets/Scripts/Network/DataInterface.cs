using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts.Network
{
    interface DataInterface
    {
        DataPackage AddDataHead(int a);
        DataPackage AddHealth(int hud);
        DataPackage AddPositon(Vector3 Pos);

        //void PackUp();
    }
}
