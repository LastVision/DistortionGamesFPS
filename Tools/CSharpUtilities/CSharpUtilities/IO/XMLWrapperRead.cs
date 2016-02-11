using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Globalization;

namespace CSharpUtilities
{
    public class XMLWrapperRead
    {
        private NumberFormatInfo myNumberFormat = new CultureInfo("en-US", false).NumberFormat;
        private XmlDocument myDocument = new XmlDocument();
        public XMLWrapperRead()
        {

        }

        public XmlDocument Open(string aFilePath)
        {
            myDocument.Load(aFilePath);
            return myDocument;
        }

        public XmlNode FindFirstElement()
        {
            XmlNodeList nodes = myDocument.SelectNodes("/*");
            return nodes.Item(0);
        }

        public XmlNode FindFirstChildElement(XmlNode aNode)
        {
            XmlNode child = aNode.ChildNodes[0];
            return child;
        }

        public XmlNode FindFirstChildElement(XmlNode aNode, string aName)
        {
            foreach (XmlNode e in aNode.ChildNodes)
            {
                if (e.Name == aName)
                {
                    return e;
                }
            }
            return null;
        }

        public XmlNode FindNextSiblingElement(XmlNode aNode)
        {
            return aNode.NextSibling;
        }

        public XmlNode FindNextSiblingElement(XmlNode aNode, string aName)
        {
            for (XmlNode e = aNode; e != null; e = e.NextSibling)
            {
                if (e.Name == aName)
                {
                    return e;
                }
            }
            return null;
        }

        public void ReadAttribute(XmlNode aNode, string aAttribute, ref float aValue)
        {
            foreach (XmlAttribute att in aNode.Attributes)
            {
                if (att.Name == aAttribute)
                {
                    string formatedValue = att.Value;
                    formatedValue = formatedValue.Replace("f", "");
                    try
                    {
                        aValue = float.Parse(formatedValue, myNumberFormat);
                    }
                    catch (FormatException)
                    {
                        formatedValue = formatedValue.Replace(".", ",");
                        aValue = float.Parse(formatedValue, myNumberFormat);
                    }
                }
            }
        }

        public void ReadAttribute(XmlNode aNode, string aAttribute, ref int aValue)
        {
            foreach (XmlAttribute att in aNode.Attributes)
            {
                if (att.Name == aAttribute)
                {
                    aValue = int.Parse(att.Value, myNumberFormat);
                }
            }
        }

        public void ReadAttribute(XmlNode aNode, string aAttribute, ref string aValue)
        {
            foreach (XmlAttribute att in aNode.Attributes)
            {
                if (att.Name == aAttribute)
                {
                    aValue = att.Value;
                }
            }
        }

        public void ReadAttribute(XmlNode aNode, string aAttribute, ref bool aValue)
        {
            foreach (XmlAttribute att in aNode.Attributes)
            {
                if (att.Name == aAttribute)
                {
                    if (att.Value == "true" || att.Value == "True" || att.Value == "TRUE")
                    {
                        aValue = true;
                    }
                    else
                    {
                        aValue = false;
                    }
                }
            }
        }

        public void ReadAttribute(XmlNode aNode, string aAttribute, ref double aValue)
        {
            foreach (XmlAttribute att in aNode.Attributes)
            {
                if (att.Name == aAttribute)
                {
                    string formatedValue = att.Value;
                    try
                    {
                        aValue = double.Parse(formatedValue, myNumberFormat);
                    }
                    catch (FormatException)
                    {
                        formatedValue = formatedValue.Replace(".", ",");
                        aValue = double.Parse(formatedValue, myNumberFormat);
                    }
                }
            }
        }
    }
}
