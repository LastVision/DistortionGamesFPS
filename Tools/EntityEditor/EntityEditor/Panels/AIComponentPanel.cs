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
    public enum eAIMode
    {
        KEEP_DISTANCE,
        ESCAPE_THEN_RETURN,
        KAMIKAZE
    };

    public class AIComponentPanel : BasePanel
    {
        private Entity.AIComponentData myAIComponent;

        private MinMaxComponent mySpeed;
        private MinMaxComponent myTimeToNextDecision;
        private NumericTextComponent myFollowEntity;
        private NumericTextComponent myAvoidanceDistance;
        private NumericTextComponent myAiTurnRate;
        private Vector3Component myAvoidanceOffset;
        private DropDownComponent myAIMode;
        private Label myAvoidanceLabel = new Label();

        public AIComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "AI Component";
        }

        protected override void IntitalizeControls()
        {
            mySpeed = new MinMaxComponent(new Point(Location.X, Location.Y + 10), new Size(Size.Width, 13), "Speed");
            mySpeed.AddTextChangeEvent(this.PanelDataChanged);
            mySpeed.BindToPanel(this);
            mySpeed.Show();

            myTimeToNextDecision = new MinMaxComponent(new Point(Location.X, Location.Y + 30), new Size(Size.Width, 13), "TimeToNextDecision");
            myTimeToNextDecision.AddTextChangeEvent(this.PanelDataChanged);
            myTimeToNextDecision.BindToPanel(this);
            myTimeToNextDecision.Show();

            myFollowEntity = new NumericTextComponent(new Point(Location.X, Location.Y + 50), new Size(275, 13), "Follow Entity", false);
            myFollowEntity.AddTextChangeEvent(this.PanelDataChanged);
            myFollowEntity.BindToPanel(this);
            myFollowEntity.Show();

            myAIMode = new DropDownComponent(new Point(Location.X, Location.Y + 70), new Size(150, 13), "AIMode");
            myAIMode.AddSelectedIndexChangeEvent(this.PanelDataChanged);
            myAIMode.BindToPanel(this);
            myAIMode.Show();

            myAiTurnRate = new NumericTextComponent(new Point(Location.X, Location.Y + 100), new Size(275, 13), "AITurnRate");
            myAiTurnRate.AddTextChangeEvent(this.PanelDataChanged);
            myAiTurnRate.BindToPanel(this);
            myAiTurnRate.Show();

            myAvoidanceLabel.Text = "Avoidance";
            myAvoidanceLabel.Location = new Point(Location.X, Location.Y + 120);
            myAvoidanceLabel.Size = new Size(100, 13);
            myAvoidanceLabel.Show();
            this.Controls.Add(myAvoidanceLabel);

            myAvoidanceDistance = new NumericTextComponent(new Point(Location.X, Location.Y + 140), new Size(150, 13), "Distance");
            myAvoidanceDistance.AddTextChangeEvent(this.PanelDataChanged);
            myAvoidanceDistance.BindToPanel(this);
            myAvoidanceDistance.Show();


            myAvoidanceOffset = new Vector3Component(new Point(Location.X, Location.Y + 160), new Size(275, 13), "Offset");
            myAvoidanceOffset.AddTextChangeEvent(this.PanelDataChanged);
            myAvoidanceOffset.BindToPanel(this);
            myAvoidanceOffset.Show();

            myAIMode.AddItem("Keep Distance");
            myAIMode.AddItem("Escape then return");
            myAIMode.AddItem("Kamikaze");
        }

        public void Load(Entity.AIComponentData aAIComponent)
        {
            myHasLoadedComponent = false;
            myAIComponent = aAIComponent;
            LoadSettings();
            myHasLoadedComponent = true;
        }

        public void Save()
        {
            SaveSettings();
        }

        protected override void LoadSettings()
        {
            mySpeed.SetMinValue(myAIComponent.mySpeed.myX);
            mySpeed.SetMaxValue(myAIComponent.mySpeed.myY);

            myTimeToNextDecision.SetMinValue(myAIComponent.myTimeToNextDecision.myX);
            myTimeToNextDecision.SetMaxValue(myAIComponent.myTimeToNextDecision.myY);

            myFollowEntity.GetTextBox().Text = myAIComponent.myEntityToFollow;

            if (myAIComponent.myAIMode == 0)
            {
                myAIMode.GetDropDown().SelectedIndex = myAIComponent.myAIMode;
            }
            else
            {
                myAIMode.GetDropDown().SelectedIndex = myAIComponent.myAIMode - 1;
            }

            myAiTurnRate.GetTextBox().Text = myAIComponent.myAITurnRate.ToString();

            myAvoidanceDistance.GetTextBox().Text = myAIComponent.myAvoidanceDistance.ToString();

            myAvoidanceOffset.SetPosition(myAIComponent.myAvoidanceOffset);
        }

        protected override void SaveSettings()
        {
            myAIComponent.mySpeed.myX = mySpeed.GetMinValue();
            myAIComponent.mySpeed.myY = mySpeed.GetMaxValue();

            myAIComponent.myTimeToNextDecision.myX = myTimeToNextDecision.GetMinValue();
            myAIComponent.myTimeToNextDecision.myY = myTimeToNextDecision.GetMaxValue();

            myAIComponent.myEntityToFollow = myFollowEntity.GetTextBox().Text;
            myAIComponent.myAIMode = myAIMode.GetDropDown().SelectedIndex + 1;

            myAIComponent.myAITurnRate = StringUtilities.ToFloat(myAiTurnRate.GetTextBox().Text);

            float avoidanceDistance = 0;
            if (myAvoidanceDistance.GetTextBox().Text != "") 
                avoidanceDistance = StringUtilities.ToFloat(myAvoidanceDistance.GetTextBox().Text);
            myAIComponent.myAvoidanceDistance = avoidanceDistance;
            myAIComponent.myAvoidanceOffset.myX = myAvoidanceOffset.GetX();
            myAIComponent.myAvoidanceOffset.myY = myAvoidanceOffset.GetY();
            myAIComponent.myAvoidanceOffset.myZ = myAvoidanceOffset.GetZ();

            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            eForm.SetAIComponent(myAIComponent);
        }
    }
}
