using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneSetUp : MonoBehaviour
{
    public GameObject GrouondPrefab;

    private void Start()
    {
        GameObject LocalPlayer = (GameObject)Instantiate(GrouondPrefab);
    }

}
