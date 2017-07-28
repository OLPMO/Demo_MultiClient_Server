using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Assets.Scripts.Player;
using Assets.Scripts.NetWorkModule;

public class PlayerController : MonoBehaviour {

    //这里负责保存角色的一些组件信息
    public float speed = 100;
    Rigidbody rigidbody;
    public GameObject BulletSpawn;
    public GameObject BulletPrefab;
    private float AsyncTime;


    //这个成员负责管理是一些角色的属性信息，例如坐标，血量，技能等
    private Role player;

    public bool bIsLocal = true;  //这个成员负责判断是否是本地玩家，用于事件同步时使用
    public float fSyncTime;  //这个成员用于心跳同步中的同步周期长度


	// Use this for initialization
	void Start ()
    {
        player = PlayerManager.GetSingleton().GetLocalPlayer();
        rigidbody = GetComponent<Rigidbody>();
	}
	
	// Update is called once per frame
	void Update ()
    {
        var x = Input.GetAxis("Horizontal") * Time.deltaTime * speed;
        var z = Input.GetAxis("Vertical") * Time.deltaTime * speed;

        Move(x,0,z);
	}

    //用于移动，并将当前角色的位置信息同步到本地角色属性信息管理类，Role中
    public void Move(float x,float y,float z)
    {
        AsyncTime += Time.deltaTime;


        Vector3 MoveDistance = new Vector3(x, y, z);
        rigidbody.AddForce(MoveDistance);
        player.GetRoleTransform().SetPostion(transform.position);

        if (AsyncTime >= 0.2)
        {
            if (bIsLocal == true)
            {
                NetSend.GetSingleton().SendMessage(transform.position);
            }
            if (bIsLocal == false)
            {
                var RemotePosition = NetRec.GetSingleton().ReceiveMessage();
                transform.position = RemotePosition;
            }
        }
    }
}
