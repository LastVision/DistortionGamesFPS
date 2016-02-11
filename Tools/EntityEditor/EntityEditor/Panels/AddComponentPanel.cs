using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using CSharpUtilities.Components;
using CSharpUtilities;

namespace EntityEditor.Panels
{
    public enum eComponentType
    {
        AIComponent,
        CollisionComponent,
        GraphicsComponent,
        ShootingComponent,
        HealthComponent,
        PhysicsComponent,
        BulletComponent,
        PowerUpComponent,
        ParticleEmitterComponent,
        SoundComponent,
        StreakEmitterComponent
    };

    public class AddComponentPanel : BasePanel
    {
        private DropDownComponent myComponents;
        private Button myAddButton = new Button();
        private Entity.EntityData myEntity;

        public AddComponentPanel(Point aLocation, Size aSize, Form aParent)
            : base(aLocation, aSize, aParent)
        {
            myPropertyTitle.Text += "Add Components";
        }

        protected override void IntitalizeControls()
        {
            myComponents = new DropDownComponent(new Point(Location.X, Location.Y + 10), new Size(245, 13), "Component");
            myComponents.BindToPanel(this);
            myComponents.Show();

            myAddButton.Text = "Add";
            myAddButton.Location = new Point(Location.X + 250, Location.Y + 10);
            myAddButton.Size = new Size(40, 20);
            myAddButton.Click += new EventHandler(AddButton_Click);
            myAddButton.Show();

            this.Controls.Add(myAddButton);
        }

        public void Load(Entity.EntityData aEntity)
        {
            myEntity = aEntity;
            LoadSettings();
        }

        protected override void LoadSettings()
        {
            myComponents.GetDropDown().Items.Clear();
            if (myEntity.myAIComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.AIComponent);
            }
            if (myEntity.myBulletComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.BulletComponent);
            }
            if (myEntity.myCollisionComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.CollisionComponent);
            }
            if (myEntity.myGraphicsComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.GraphicsComponent);
            }
            if (myEntity.myHealthComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.HealthComponent);
            }
            if (myEntity.myParticleEmitterComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.ParticleEmitterComponent);
            }
            if (myEntity.myPhysicsComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.PhysicsComponent);
            }
            if (myEntity.myPowerUpComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.PowerUpComponent);
            }
            if (myEntity.myShootingComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.ShootingComponent);
            }
            if (myEntity.mySoundComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.SoundComponent);
            }
            if (myEntity.myStreakEmitterComponent.myIsActive == false)
            {
                myComponents.AddItem(eComponentType.StreakEmitterComponent);
            }
        }

        protected override void SaveSettings()
        {
            EntityEditorForm eForm = (EntityEditorForm)myOwnerForm;
            if(myEntity.myAIComponent.myIsActive == true)
            {
                eForm.SetAIComponent(myEntity.myAIComponent);
            }
            if (myEntity.myCollisionComponent.myIsActive == true)
            {
                eForm.SetCollisionComponent(myEntity.myCollisionComponent);
            }
            if (myEntity.myGraphicsComponent.myIsActive == true)
            {
                eForm.SetGraphicsComponent(myEntity.myGraphicsComponent);
            }
            if (myEntity.myShootingComponent.myIsActive == true)
            {
                eForm.SetShootingComponent(myEntity.myShootingComponent);
            }
            if (myEntity.myHealthComponent.myIsActive == true)
            {
                eForm.SetHealthComponent(myEntity.myHealthComponent);
            }
            if (myEntity.myPhysicsComponent.myIsActive == true)
            {
                eForm.SetPhysicsComponent(myEntity.myPhysicsComponent);
            }
            if (myEntity.myBulletComponent.myIsActive == true)
            {
                eForm.SetBulletComponent(myEntity.myBulletComponent);
            }
            if (myEntity.myPowerUpComponent.myIsActive == true)
            {
                eForm.SetPowerUpComponent(myEntity.myPowerUpComponent);
            }
            if (myEntity.myParticleEmitterComponent.myIsActive == true)
            {
                eForm.SetParticleEmitterComponent(myEntity.myParticleEmitterComponent);
            }
            if (myEntity.mySoundComponent.myIsActive == true)
            {
                eForm.SetSoundComponent(myEntity.mySoundComponent);
            }
            if (myEntity.myStreakEmitterComponent.myIsActive == true)
            {
                eForm.SetStreakEmitterComponent(myEntity.myStreakEmitterComponent);
            }
            eForm.DisplayEntityData();
        }

        private void AddButton_Click(object sender, EventArgs e)
        {
            if (myComponents.GetDropDown().SelectedItem == null) return;

            switch((eComponentType)myComponents.GetDropDown().SelectedItem)
            {
                case eComponentType.AIComponent:
                    myEntity.myAIComponent.myIsActive = true;
                    break;
                case eComponentType.CollisionComponent:
                    myEntity.myCollisionComponent.myIsActive = true;
                    break;
                case eComponentType.GraphicsComponent:
                    myEntity.myGraphicsComponent.myIsActive = true;
                    break;
                case eComponentType.ShootingComponent:
                    myEntity.myShootingComponent.myIsActive = true;
                    break;
                case eComponentType.HealthComponent:
                    myEntity.myHealthComponent.myIsActive = true;
                    break;
                case eComponentType.PhysicsComponent:
                    myEntity.myPhysicsComponent.myIsActive = true;
                    break;
                case eComponentType.BulletComponent:
                    myEntity.myBulletComponent.myIsActive = true;
                    break;
                case eComponentType.PowerUpComponent:
                    myEntity.myPowerUpComponent.myIsActive = true;
                    break;
                case eComponentType.ParticleEmitterComponent:
                    myEntity.myParticleEmitterComponent.myIsActive = true;
                    break;
                case eComponentType.SoundComponent:
                    myEntity.mySoundComponent.myIsActive = true;
                    break;
                case eComponentType.StreakEmitterComponent:
                    myEntity.myStreakEmitterComponent.myIsActive = true;
                    break;
                default:
                    DL_Debug.GetInstance.DL_ErrorMessage("The component " 
                        + (string)myComponents.GetDropDown().SelectedItem + " is not currently supported.");
                    break;
            }

            SaveSettings();
            LoadSettings();

            if (myComponents.GetDropDown().Items.Count > 0)
            {
                myComponents.GetDropDown().SelectedIndex = 0;
            }
        }
    }
}
