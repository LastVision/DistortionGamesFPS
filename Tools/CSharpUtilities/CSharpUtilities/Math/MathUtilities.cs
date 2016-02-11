using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpUtilities
{
    public class MathUtilities
    {
        public static float Remap(float aValue, float aOrginalMin, float aOrginalMax, float aNewMin, float aNewMax)
        {
            return (aValue - aOrginalMin) / (aOrginalMax - aOrginalMin) * (aNewMax - aNewMin) + aNewMin;
        }
    }
}
