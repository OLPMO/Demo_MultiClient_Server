using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts.Player
{
    class RoleTransform
    {
        private Vector3 Pos;

        public void SetPostion(Vector3 CurrentPostion)
        {
            Pos = CurrentPostion;
        }

        public Vector3 GetPostion()
        {
            return Pos;
        }

    }
}
