using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Login : MonoBehaviour
{
    public void OnLoginClicked()
    {
        SceneManager.LoadScene("GameScene",LoadSceneMode.Single);
    }
}
