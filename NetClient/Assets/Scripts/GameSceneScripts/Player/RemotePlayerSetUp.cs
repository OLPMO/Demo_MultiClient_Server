using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RemotePlayerSetUp : MonoBehaviour
{
    public GameObject RemotePlayerSpawn;
    public GameObject RemotePlayerPrefab;

    private void Start()
    {
        GameObject RemotePlayer = (GameObject)Instantiate(RemotePlayerPrefab);
        RemotePlayer.transform.position = RemotePlayerSpawn.transform.position;
        

        RemotePlayer.GetComponent<Renderer>().material.color = Color.blue;
        Debug.Log(RemotePlayer.GetComponent<PlayerController>().GetIsLocal());
        Debug.Log("RemotePlayerSetup");
    }

}
