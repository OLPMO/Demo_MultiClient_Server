using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using UnityEngine.SceneManagement;
using Assets.Scripts.Network;
using UnityEngine.UI;
using NetParsePackageService;
using System.Threading;

public class LoginAndTurnToGameScene : MonoBehaviour
{

    public InputField AccountInput;
    public InputField PasswordInput;
    private int LoginErrorWindowNum = -10;

    private void Start()
    {

    }

    public void OnLoginClicked()
    {
        SendData LoginData = new SendData(-1);
        LoginData.GetDataPackage().AddLoginInfo(AccountInput.text, PasswordInput.text);
        int sendNum = LoginData.Send();
        Debug.Log("Here we send the login data to the server! The send Num is: " + sendNum);



        PackageMetaData LoginRecvPackage = ReceiveData.GetSingleton().GetLatestPackage("Login");

        int times = 0;
        while (true)
        {
            if (times >= 3) break;
            if (LoginRecvPackage == null)
                Thread.Sleep(1000);
            else
                break;
            LoginRecvPackage = ReceiveData.GetSingleton().GetLatestPackage("Login");
            times += 1;
            Debug.Log("Waitting for server login message!");
        }

        if (LoginRecvPackage == null)
        {
            Debug.Log("Please check you internet ,it's seems disconnected!");
            return;
        }

        if (Convert.ToBoolean(LoginRecvPackage.Get("LoginReturn")) == true)
        {
            //if (AccountInput.text == "123456" && PasswordInput.text == "654321")
            //{
            SceneManager.LoadScene("GameScene", LoadSceneMode.Single);
        }
        else
        {
            Debug.Log("密码错误");
        }
    }


    //private void OnGUI()
    //{
    //    float width;
    //    float height;
    //    float leftBorder;
    //    float rightBorder;
    //    float topBorder;
    //    float downBorder;
    //    //the up right corner
    //    Vector3 cornerPos = Camera.main.ViewportToWorldPoint(new Vector3(1f, 1f, Mathf.Abs(-Camera.main.transform.position.z)));

    //    leftBorder = Camera.main.transform.position.x - (cornerPos.x - Camera.main.transform.position.x);
    //    rightBorder = cornerPos.x;
    //    topBorder = cornerPos.y;
    //    downBorder = Camera.main.transform.position.y - (cornerPos.y - Camera.main.transform.position.y);

    //    width = rightBorder - leftBorder;
    //    height = topBorder - downBorder;
    //    GUI.Window(LoginErrorWindowNum, new Rect(width / 2 - 40, height / 2 - 20, width / 2 + 40, height / 2 + 20), DisplayLoginMessage, "账号或密码错误，请重新输入！");
    //}

    private void DisplayLoginMessage(int id)
    {
        Debug.Log("Here we know the Login account or password is error!");
    }
}
