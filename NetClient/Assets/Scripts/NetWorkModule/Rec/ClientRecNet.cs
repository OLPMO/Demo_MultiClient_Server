using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Assets.Scripts.Player;
using System.Net;
using System.Net.Sockets;

public class ClientRecNet :MonoBehaviour
{
    private Role RemotePlayer;
    private NetRec netRec;

    public ClientRecNet()
    {
        netRec = NetRec.GetSingleton();
    }

    public  Vector3 GetTransform()
    {
        return netRec.RecTransform();
    }

}
