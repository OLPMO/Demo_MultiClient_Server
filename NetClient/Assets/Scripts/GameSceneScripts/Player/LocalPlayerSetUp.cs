using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LocalPlayerSetUp : MonoBehaviour
{
    public GameObject LocalPlayerSpawn;
    public GameObject LocalPlayerPrefab;

    private void Start()
    {
        GameObject LocalPlayer = (GameObject)Instantiate(LocalPlayerPrefab);
        LocalPlayer.transform.position = LocalPlayerSpawn.transform.position;
        LocalPlayer.GetComponent<PlayerController>().SetIsLocalTrue();

        Debug.Log("LocalPlayerSetup");
    }

}
