using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Assets.Scripts.Player
{
    class Role
    {
        public bool bIsLocal { get; set; }

        private RoleTransform roleTransform;

        public Role()
        {
            roleTransform = new RoleTransform();
        }

        public RoleTransform GetRoleTransform()
        {
            return roleTransform;
        }
    }
}
