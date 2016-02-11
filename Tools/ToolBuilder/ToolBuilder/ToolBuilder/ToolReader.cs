using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using System.Xml;
using CSharpUtilities;
using CSharpUtilities.Components;
using CSharpUtilities.Components.IO;

namespace ToolBuilder.ToolBuilder
{
    public class ToolReader
    {
        private XMLWrapperRead myXMLReader = new XMLWrapperRead();

        private List<BaseComponent> myComponents = new List<BaseComponent>();

        private String myCurrentPanelName;

        public ToolReader(String aPath, ref Panel aMainWindow, ref Panel aPropertyWindow)
        {
            ReadBuildDoc(aPath, ref aMainWindow, ref aPropertyWindow);
        }

        public List<BaseComponent> GetComponents()
        {
            return myComponents;
        }

        private void ReadBuildDoc(String aPath, ref Panel aMainWindow, ref Panel aPropertyWindow)
        {
            String path = Directory.GetCurrentDirectory()  + "/"+ aPath;
            path = path.Replace("\\", "/");
            myXMLReader.Open(path);

            XmlNode root = myXMLReader.FindFirstElement();
            for (XmlNode panel = myXMLReader.FindFirstChildElement(root);
                panel != null; panel = myXMLReader.FindNextSiblingElement(panel))
            {
                ReadPanel(panel, ref aMainWindow, ref aPropertyWindow);
            }
        }

        private void ReadPanel(XmlNode aNode, ref Panel aMainWindow, ref Panel aPropertyWindow)
        {
            myXMLReader.ReadAttribute(aNode, "name", ref myCurrentPanelName);
            for (XmlNode component = myXMLReader.FindFirstChildElement(aNode); component != null;
                component = myXMLReader.FindNextSiblingElement(component))
            {
                ReadComponent(component);
            }
            for (int i = 0; i < myComponents.Count; ++i)
            {
                if (myComponents[i].GetPanelName() == "PropertyWindow")
                {
                    BuildPanel(ref aPropertyWindow, i);
                }
                else
                {
                    BuildPanel(ref aMainWindow, i);
                }
            }
        }

        private void ReadComponent(XmlNode aNode)
        {
            String type = "";
            ComponentIO IO = new ComponentIO();
            IO.myIOType = eIOType.IN;
            IO.myIOComponentType = "";
            IO.myIOElementName = "";
            IO.myIOAttribute = new String[8];
            for (int i = 0; i < 8; i++)
            {
                IO.myIOAttribute[i] = "";
            }

            myXMLReader.ReadAttribute(aNode, "type", ref type);

            XmlNode IOnode = myXMLReader.FindFirstChildElement(aNode);
            if (IOnode != null)
            {
                if (IOnode.Name == "InputOutput")
                {
                    IO.myIOType = eIOType.INOUT;
                }
                else if (IOnode.Name == "Output")
                {
                    IO.myIOType = eIOType.OUT;
                }
                else
                {
                    IO.myIOType = eIOType.IN;
                }

                myXMLReader.ReadAttribute(IOnode, "type", ref IO.myIOComponentType);
                myXMLReader.ReadAttribute(IOnode, "elementName", ref IO.myIOElementName);
                myXMLReader.ReadAttribute(IOnode, "attribute0", ref IO.myIOAttribute[0]);
                myXMLReader.ReadAttribute(IOnode, "attribute1", ref IO.myIOAttribute[1]);
                myXMLReader.ReadAttribute(IOnode, "attribute2", ref IO.myIOAttribute[2]);
                myXMLReader.ReadAttribute(IOnode, "attribute3", ref IO.myIOAttribute[3]);
                myXMLReader.ReadAttribute(IOnode, "attribute4", ref IO.myIOAttribute[4]);
                myXMLReader.ReadAttribute(IOnode, "attribute5", ref IO.myIOAttribute[5]);
                myXMLReader.ReadAttribute(IOnode, "attribute6", ref IO.myIOAttribute[6]);
                myXMLReader.ReadAttribute(IOnode, "attribute7", ref IO.myIOAttribute[7]);

            }
            BuildComponent(type, IO);
        }

        private void BuildComponent(string aType, ComponentIO aIO)
        {
            BaseComponent newComponent = null;
            switch(aType)
            {
                case "DLLPreview":
                    newComponent = new DLLPreviewComponent(new Point(0, 0), new Size(700, 530), "DLLPreview", myCurrentPanelName);
                    break;
                case "Vector3":
                    newComponent = new Vector3Component(new Point(0, 0), new Size(120, 13), aIO.myIOElementName, myCurrentPanelName);
                    break;
            }
            if (newComponent != null)
            {
                myComponents.Add(newComponent);
            }
        }

        private void BuildPanel(ref Panel aPanel, int aIndex)
        {
            myComponents[aIndex].BindToPanel(aPanel);
            myComponents[aIndex].Show();
        }
    }
}
