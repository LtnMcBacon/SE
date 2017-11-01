using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace SkillCreator
{
    /// <summary>
    /// Interaction logic for AddSkillWindow.xaml
    /// </summary>
    public partial class AddSkillWindow : Window 
    {
        private MainWindow.Skill tempSkill;

        public DescriptionPage descriptionPage;
        public BoonsPage boonsPage;
        public BanesPage banesPage;
        public AnimationPage animationPage;
        public ParticlePage particlePage;
        public TweakingPage tweakingPage;

        public AddSkillWindow()
        {
            InitializeComponent();
            WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;

            descriptionPage = new DescriptionPage();
            boonsPage       = new BoonsPage();
            banesPage       = new BanesPage();
            animationPage   = new AnimationPage();
            particlePage    = new ParticlePage();
            tweakingPage    = new TweakingPage();

            tempSkill           = new MainWindow.Skill();
            tempSkill.name      = "";
            tempSkill.atkType   = 0;
            tempSkill.element   = 0;
            tempSkill.boon      = 0;
            tempSkill.bane      = 0;
            tempSkill.animation = 0;
            tempSkill.particle  = 0;

            tempSkill.skillDamage       = 0;
            tempSkill.boonEffectValue   = 0;
            tempSkill.boonRange         = 0;
            tempSkill.boonDuration      = 0;
            tempSkill.baneEffectValue   = 0;
            tempSkill.baneRange         = 0;
            tempSkill.baneDuration      = 0;
        }
        public AddSkillWindow(MainWindow.Skill skill)
        {
            InitializeComponent();
            WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;
            CreateSkillButton.Content = "Edit";
            this.Title = "☢ Edit your skill! ☢";

            descriptionPage = new DescriptionPage();
            boonsPage = new BoonsPage();
            banesPage = new BanesPage();
            animationPage = new AnimationPage();
            particlePage = new ParticlePage();
            tweakingPage = new TweakingPage();

            tempSkill = new MainWindow.Skill();
            tempSkill.name              = skill.name;
            descriptionPage.SkillName.Text   = skill.name;
            tempSkill.atkType           = skill.atkType;
            descriptionPage._atkType    = skill.atkType;
            tempSkill.element           = skill.element;
            descriptionPage._element    = skill.element;
            tempSkill.boon              = skill.boon;
            boonsPage._Boon             = skill.boon;
            tempSkill.bane              = skill.bane;
            banesPage._Bane             = skill.bane;
            //tempSkill.animation         = skill.animation;
            //animationPage._animation    = skill.animation;
            //tempSkill.particle          = skill.particle;
            //particlePage._particle      = skill.particle;

            tempSkill.skillDamage           = skill.skillDamage;
            descriptionPage._skillDamage    = skill.skillDamage;

            tempSkill.boonEffectValue       = skill.boonEffectValue;
            boonsPage._effectValue          = skill.boonEffectValue;
            tempSkill.boonRange             = skill.boonRange;
            boonsPage._rangeValue           = skill.boonRange;
            tempSkill.boonDuration          = skill.boonDuration;
            boonsPage._duration             = skill.boonDuration;

            tempSkill.baneEffectValue       = skill.baneEffectValue;
            banesPage._effectValue          = skill.baneEffectValue;
            tempSkill.baneRange             = skill.baneRange;
            banesPage._rangeValue           = skill.baneRange;
            tempSkill.baneDuration          = skill.baneDuration;
            banesPage._duration             = skill.baneDuration;

            sumSkillName.Text       = skill.name.ToString();

            if (skill.atkType == 0)
            {
                sumAtkType.Text = "SelfCast";
            }
            else if (skill.atkType == 1)
            {
                sumAtkType.Text = "Melee";
            }
            else if (skill.atkType == 2)
            {
                sumAtkType.Text = "Ranged";
            }
            else if (skill.atkType == 3)
            {
                sumAtkType.Text = "Area";
            }

            if (skill.element == 0)
            {
                sumElementType.Text = "Physical";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Physical.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (skill.element == 1)
            {
                sumElementType.Text = "Ranged";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Ranged.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (skill.element == 2)
            {
                sumElementType.Text = "Magical";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Magical.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (skill.element == 3)
            {
                sumElementType.Text = "Fire";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Fire.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (skill.element == 4)
            {
                sumElementType.Text = "Water";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Water.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (skill.element == 5)
            {
                sumElementType.Text = "Nature";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Nature.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }

            if (skill.boon == 0)
            {
                sumBoonType.Text = "None";
            }
            else if (skill.boon == 1)
            {
                sumBoonType.Text = "Damage";
            }
            else if (skill.boon == 2)
            {
                sumBoonType.Text = "Knockback";
            }
            else if (skill.boon == 3)
            {
                sumBoonType.Text = "Stun";
            }
            else if (skill.boon == 4)
            {
                sumBoonType.Text = "Root";
            }
            else if (skill.boon == 5)
            {
                sumBoonType.Text = "Protection";
            }
            else if (skill.boon == 6)
            {
                sumBoonType.Text = "Physical resistance";
            }
            else if (skill.boon == 7)
            {
                sumBoonType.Text = "Magical resistance";
            }
            else if (skill.boon == 8)
            {
                sumBoonType.Text = "Fire Resistance";
            }
            else if (skill.boon == 9)
            {
                sumBoonType.Text = "Water resistance";
            }
            else if (skill.boon == 10)
            {
                sumBoonType.Text = "Nature resistance";
            }
            else if (skill.boon == 11)
            {
                sumBoonType.Text = "Castspeed";
            }
            else if (skill.boon == 12)
            {
                sumBoonType.Text = "Swiftness";
            }
            else if (skill.boon == 13)
            {
                sumBoonType.Text = "Slow";
            }
            else if (skill.boon == 14)
            {
                sumBoonType.Text = "Invurnability";
            }

            if (skill.bane == 0)
            {
                sumBaneType.Text = "None";
            }
            else if (skill.bane == 1)
            {
                sumBaneType.Text = "Damage";
            }
            else if (skill.bane == 2)
            {
                sumBaneType.Text = "Stun";
            }
            else if (skill.bane == 3)
            {
                sumBaneType.Text = "Root";
            }
            else if (skill.bane == 4)
            {
                sumBaneType.Text = "Bloodletting";
            }
            else if (skill.bane == 5)
            {
                sumBaneType.Text = "Uncover";
            }
            else if (skill.bane == 6)
            {
                sumBaneType.Text = "Physical resistance";
            }
            else if (skill.bane == 7)
            {
                sumBaneType.Text = "Magical esistance";
            }
            else if (skill.bane == 8)
            {
                sumBaneType.Text = "Fire Resistance";
            }
            else if (skill.bane == 9)
            {
                sumBaneType.Text = "Water resistance";
            }
            else if (skill.bane == 10)
            {
                sumBaneType.Text = "Nature resistance";
            }
            else if (skill.bane == 11)
            {
                sumBaneType.Text = "Slow";
            }
            //sumAnimationType.Text   = skill.animation;
            //sumParticleType.Text     = skill.particle;

            sumTotDmg.Text              = skill.skillDamage.ToString() + " total dmg";
            sumBaseDmg.Text             = skill.skillDamage.ToString() + " base dmg";
            sumBoonEffectValue.Text     = skill.boonEffectValue.ToString() +"% Effect";
            sumBoonRangeValue.Text      = skill.boonRange.ToString() + "m Range";
            sumBoonDurationValue.Text   = skill.boonDuration.ToString() + "sec";
            sumBaneEffectValue.Text     = skill.baneEffectValue.ToString() + "% Effect";
            sumBaneRangeValue.Text      = skill.baneRange.ToString() + "m Range";
            sumBaneDurationValue.Text   = skill.baneDuration.ToString() + "sec";

            descriptionPage.postInitializer();
            boonsPage.postInitializer();
            banesPage.postInitializer();
            //animationPage.postInitializer();
            //particlePage.postInitializer();
            //tweakingPage.postInitializer();

        }

        //Exit
        private void ExitCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }
        private void ExitCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void addSkill()
        {
            MainWindow mw = Owner as MainWindow;
            int test = mw.Menu_List.Items.Count;

            if (CreateSkillButton.Content.ToString() == "Add Skill")
            {
                tempSkill.name = descriptionPage.SkillName.Text;

                tempSkill.atkType = descriptionPage._atkType;
                tempSkill.element = descriptionPage._element;
                tempSkill.boon = boonsPage._Boon;
                tempSkill.bane = banesPage._Bane;
                //tempSkill.animation         = animationPage._Animation;
                //tempSkill.particle          = particlePage._Particle;

                tempSkill.skillDamage = descriptionPage._skillDamage;
                tempSkill.boonEffectValue = boonsPage._effectValue;
                tempSkill.boonRange = boonsPage._rangeValue;
                tempSkill.boonDuration = boonsPage._duration;

                tempSkill.baneEffectValue = banesPage._effectValue;
                tempSkill.baneRange = banesPage._rangeValue;
                tempSkill.baneDuration = banesPage._duration;

                mw.skills.Add(tempSkill);

                mw.Menu_List.Items.Add(tempSkill.name);
            }
            else if (CreateSkillButton.Content.ToString() == "Edit")
            {
                tempSkill.name = descriptionPage.SkillName.Text;

                tempSkill.atkType = descriptionPage._atkType;
                tempSkill.element = descriptionPage._element;
                tempSkill.boon = boonsPage._Boon;
                tempSkill.bane = banesPage._Bane;
                //tempSkill.animation         = animationPage._Animation;
                //tempSkill.particle          = particlePage._Particle;

                tempSkill.skillDamage = descriptionPage._skillDamage;
                tempSkill.boonEffectValue = boonsPage._effectValue;
                tempSkill.boonRange = boonsPage._rangeValue;
                tempSkill.boonDuration = boonsPage._duration;

                tempSkill.baneEffectValue = banesPage._effectValue;
                tempSkill.baneRange = banesPage._rangeValue;
                tempSkill.baneDuration = banesPage._duration;

                mw.skills[mw.Menu_List.SelectedIndex] = tempSkill;

                mw.Menu_List.Items[mw.Menu_List.SelectedIndex] = tempSkill.name;
            }
        }

        public void LogUpdate()
        {
            sumSkillName.Text = descriptionPage.SkillName.Text;

            if (descriptionPage._atkType == 0)
            {
                sumAtkType.Text = "SelfCast";
            }
            else if (descriptionPage._atkType == 1)
            {
                sumAtkType.Text = "Melee";
            }
            else if (descriptionPage._atkType == 2)
            {
                sumAtkType.Text = "Ranged";
            }
            else if (descriptionPage._atkType == 3)
            {
                sumAtkType.Text = "Area";
            }
            sumTotDmg.Text = descriptionPage._skillDamage.ToString() + " total dmg";
            sumBaseDmg.Text = descriptionPage._skillDamage.ToString() + " base dmg";

            if (descriptionPage._element == 0)
            {
                sumElementType.Text = "Physical";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Physical.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (descriptionPage._element == 1)
            {
                sumElementType.Text = "Ranged";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Ranged.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (descriptionPage._element == 2)
            {
                sumElementType.Text = "Magical";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Magical.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (descriptionPage._element == 3)
            {
                sumElementType.Text = "Fire";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Fire.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (descriptionPage._element == 4)
            {
                sumElementType.Text = "Water";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Water.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }
            else if (descriptionPage._element == 5)
            {
                sumElementType.Text = "Nature";
                BitmapImage bmp = new BitmapImage();
                bmp.BeginInit();
                bmp.UriSource = new Uri("images/Nature.png", UriKind.RelativeOrAbsolute);
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                ElementTypeImage.Source = bmp;
            }

            if (boonsPage._Boon == 0)
            {
                sumBoonType.Text = "None";
            }
            else if (boonsPage._Boon == 1)
            {
                sumBoonType.Text = "Damage";
            }
            else if (boonsPage._Boon == 2)
            {
                sumBoonType.Text = "Knockback";
            }
            else if (boonsPage._Boon == 3)
            {
                sumBoonType.Text = "Stun";
            }
            else if (boonsPage._Boon == 4)
            {
                sumBoonType.Text = "Root";
            }
            else if (boonsPage._Boon == 5)
            {
                sumBoonType.Text = "Protection";
            }
            else if (boonsPage._Boon == 6)
            {
                sumBoonType.Text = "Physical resistance";
            }
            else if (boonsPage._Boon == 7)
            {
                sumBoonType.Text = "Magical resistance";
            }
            else if (boonsPage._Boon == 8)
            {
                sumBoonType.Text = "Fire Resistance";
            }
            else if (boonsPage._Boon == 9)
            {
                sumBoonType.Text = "Water resistance";
            }
            else if (boonsPage._Boon == 10)
            {
                sumBoonType.Text = "Nature resistance";
            }
            else if (boonsPage._Boon == 11)
            {
                sumBoonType.Text = "Castspeed";
            }
            else if (boonsPage._Boon == 12)
            {
                sumBoonType.Text = "Swiftness";
            }
            else if (boonsPage._Boon == 13)
            {
                sumBoonType.Text = "Slow";
            }
            else if (boonsPage._Boon == 14)
            {
                sumBoonType.Text = "Invurnability";
            }
            sumBoonEffectValue.Text = boonsPage._effectValue.ToString() + "% Effect";
            sumBoonRangeValue.Text = boonsPage._rangeValue.ToString() + "m Range";
            sumBoonDurationValue.Text = boonsPage._duration.ToString() + "sec";

            if (banesPage._Bane == 0)
            {
                sumBaneType.Text = "None";
            }
            else if (banesPage._Bane == 1)
            {
                sumBaneType.Text = "Damage";
            }
            else if (banesPage._Bane == 2)
            {
                sumBaneType.Text = "Stun";
            }
            else if (banesPage._Bane == 3)
            {
                sumBaneType.Text = "Root";
            }
            else if (banesPage._Bane == 4)
            {
                sumBaneType.Text = "Bloodletting";
            }
            else if (banesPage._Bane == 5)
            {
                sumBaneType.Text = "Uncover";
            }
            else if (banesPage._Bane == 6)
            {
                sumBaneType.Text = "Physical resistance";
            }
            else if (banesPage._Bane == 7)
            {
                sumBaneType.Text = "Magical esistance";
            }
            else if (banesPage._Bane == 8)
            {
                sumBaneType.Text = "Fire Resistance";
            }
            else if (banesPage._Bane == 9)
            {
                sumBaneType.Text = "Water resistance";
            }
            else if (banesPage._Bane == 10)
            {
                sumBaneType.Text = "Nature resistance";
            }
            else if (banesPage._Bane == 11)
            {
                sumBaneType.Text = "Slow";
            }
            sumBaneEffectValue.Text = banesPage._effectValue.ToString() + "% Effect";
            sumBaneRangeValue.Text = banesPage._rangeValue.ToString() + "m Range";
            sumBaneDurationValue.Text = banesPage._duration.ToString() + "sec";
        }

        private void AttributeWindow_GotFocus(object sender, RoutedEventArgs e)
        {
            LogUpdate();
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var addedItems = e.AddedItems;
            if (addedItems.Count > 0)
            {
                var selectedItem = addedItems[0];
            }
        }

        private void DescriptionSelected(object sender, MouseButtonEventArgs e)
        {
            AttributeWindow.Content = descriptionPage;
            LogUpdate();
        }
        private void BoonsSelected(object sender, MouseButtonEventArgs e)
        {
            AttributeWindow.Content = boonsPage;
            LogUpdate();
        }
        private void BanesSelected(object sender, MouseButtonEventArgs e)
        {
            AttributeWindow.Content = banesPage;
            LogUpdate();
        }
        private void AnimationSelected(object sender, MouseButtonEventArgs e)
        {
            AttributeWindow.Content = animationPage;
            LogUpdate();
        }
        private void ParticleSelected(object sender, MouseButtonEventArgs e)
        {
            AttributeWindow.Content = particlePage;
            LogUpdate();
        }
        private void TweakingSelected(object sender, MouseButtonEventArgs e)
        {
            AttributeWindow.Content = particlePage;
            LogUpdate();
        }

        private void CreateSkill(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(descriptionPage.SkillName.Text))
            {
                MessageBox.Show("You forgot to set a name!", "🐷 Missing name! 🐷", MessageBoxButton.OK, MessageBoxImage.Information);
                return;
            }

            if (CreateSkillButton.Content.ToString() == "Edit")
            {
                MessageBoxResult res = MessageBox.Show("Are you sure you want to edit your skill?", "Edit? 🐑🐑🐑", MessageBoxButton.YesNo, MessageBoxImage.Information);

                switch (res)
                {
                    case MessageBoxResult.Yes:
                        break;
                    case MessageBoxResult.No:
                        return;
                }
            }

            addSkill();
            this.Close();
        }

        private void exit_application(object sender, RoutedEventArgs e)
        {
            MessageBoxResult res = MessageBox.Show("Are you sure you want to cancel?", "Cancel? ☢", MessageBoxButton.YesNo, MessageBoxImage.Information);

            switch (res)
            {
                case MessageBoxResult.Yes:
                    this.Close();
                    break;
                case MessageBoxResult.No:
                    return;
            }
        }
    }
}

