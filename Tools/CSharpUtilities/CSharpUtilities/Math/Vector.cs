using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpUtilities
{
    public struct Vector2<T>
    {
        public T myX;
        public T myY;

        public Vector2(T aX, T aY)
        {
            myX = aX;
            myY = aY;
        }
    }

    public struct Vector3<T>
    {
        public T myX;
        public T myY;
        public T myZ;

        public Vector3(T aX, T aY, T aZ)
        {
            myX = aX;
            myY = aY;
            myZ = aZ;
        }
    }

    public struct Vector4<T>
    {
        public T myX;
        public T myY;
        public T myZ;
        public T myW;

        public Vector4(T aX, T aY, T aZ, T aW)
        {
            myX = aX;
            myY = aY;
            myZ = aZ;
            myW = aW;
        }
    }
}
