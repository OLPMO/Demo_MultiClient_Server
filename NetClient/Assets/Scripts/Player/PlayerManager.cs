using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Assets.Scripts.Player;

namespace Assets.Scripts.Player
{
    class PlayerManager
    {
        private static PlayerManager PlayerManagerInstance=null;
        private static Role LocalPlayer = null;

        public static PlayerManager GetSingleton()
        {
            if (PlayerManagerInstance == null)
                PlayerManagerInstance = new PlayerManager();

            return PlayerManagerInstance;
        }

        public Role GetLocalPlayer()
        {
            if (LocalPlayer == null)
                LocalPlayer = new Role();

            LocalPlayer.bIsLocal = true;
            return LocalPlayer;
        }

        public Role CreateRemotePlayer()
        {
            Role RemotePlayer = new Role();
            RemotePlayer.bIsLocal = false;

            return RemotePlayer;
        }
    }
}
