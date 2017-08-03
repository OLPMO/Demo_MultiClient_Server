using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GlobalTimeController : MonoBehaviour
{

    private int nTimeInSync;
    private static GlobalTimeController GlobalTimeControllerInstance;
    private static readonly System.Object LockInstanceHelper = new System.Object();

    public static GlobalTimeController GetSingleton()
    {
        if (GlobalTimeControllerInstance == null)
        {
            lock (LockInstanceHelper)
            {
                if (GlobalTimeControllerInstance == null)
                    GlobalTimeControllerInstance = new GlobalTimeController();
            }
        }
        return GlobalTimeControllerInstance;
    }




    public int GetTimeInSync()
    {
        return nTimeInSync;
    }




	// Use this for initialization
	void Start ()
    {
		
	}
	
	// Update is called once per frame
	void Update ()
    {
		
	}
}
