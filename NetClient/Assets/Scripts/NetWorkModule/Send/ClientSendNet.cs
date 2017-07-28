using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Assets.Scripts.Player;
using UnityEngine;

namespace Assets.Scripts.NetWorkModule
{
    class ClientSendNet:MonoBehaviour
    {
        private Role LocalPlayer;
        private NetSend netSend;

        public ClientSendNet()
        {
            LocalPlayer = PlayerManager.GetSingleton().GetLocalPlayer();
            netSend = NetSend.GetSingleton();
        }
    }
}
