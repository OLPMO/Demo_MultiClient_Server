using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerSetUp : MonoBehaviour
{
    public GameObject RemotePlayerSpawn;
    public GameObject RemotePlayerPrefab;

    private void Start()
    {
        GameObject RemotePlayer = (GameObject)Instantiate(RemotePlayerPrefab);
        RemotePlayer.transform.position = RemotePlayerSpawn.transform.position;
        RemotePlayer.GetComponent<PlayerController>().bIsLocal = false;
    }

}
