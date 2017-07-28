using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using Assets.Scripts.NetWorkModule;

public class PlayerController : MonoBehaviour {

    //这里负责保存角色的一些组件信息
    public float speed = 100;
    Rigidbody rigidbody;
    public float fSyncTime = 0.2f;  //这个成员用于心跳同步中的同步周期长度
    private float timePast = 0f;
    private bool bIsLocal = false; //这个成员负责判断是否是本地玩家，用于事件同步时使用
    
    public void SetIsLocalTrue() { bIsLocal = true; }
    public bool GetIsLocal() { return bIsLocal; }

	// Use this for initialization
	void Start ()
    {
        rigidbody = GetComponent<Rigidbody>();
	}
	
	// Update is called once per frame
	void Update ()
    {
        var x = Input.GetAxis("Horizontal") * Time.deltaTime * speed;
        var z = Input.GetAxis("Vertical") * Time.deltaTime * speed;

        timePast += Time.deltaTime;
        if (timePast >= fSyncTime)
        {
            if (GetIsLocal() == false)
            {
                var curPos = NetworkManager.GetSingleton().GetTransform();
                transform.position = curPos;

                return;
            }
            NetworkManager.GetSingleton().SendMessageToServer(transform.position);
        }
        Move(x,0,z);
	}

   
    public void Move(float x,float y,float z)
    {
        timePast += Time.deltaTime;

        Vector3 MoveDistance = new Vector3(x, y, z);
        rigidbody.AddForce(MoveDistance);
    }
}
