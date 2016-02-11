using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpUtilities.Components.IO
{
    public enum eIOType
    {
        IN,
        OUT,
        INOUT
    };

    public class ComponentIO
    {
        public eIOType myIOType;
        public String myIOComponentType;
        public String myIOElementName;
        public String[] myIOAttribute;
    }
}
