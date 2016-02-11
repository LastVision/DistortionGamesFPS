using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using CSharpUtilities;
using CSharpUtilities.Components;

namespace EntityEditor.Panels
{
    public class SoundComponentPanel : BasePanel
    {
        private Entity.SoundComponentData mySoundComponent;
        public SoundComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {

        }

        public void Load(Entity.SoundComponentData aSoundComponentData)
        {
            mySoundComponent = aSoundComponentData;
        }

        protected override void IntitalizeControls()
        {
            myPropertyTitle.Text += "Sound Component";
        }

        protected override void LoadSettings()
        {
            
        }

        protected override void SaveSettings()
        {
            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetSoundComponent(mySoundComponent);
        }
    }
}
