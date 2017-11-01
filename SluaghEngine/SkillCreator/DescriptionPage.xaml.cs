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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SkillCreator
{
    /// <summary>
    /// Interaction logic for DescriptionPage.xaml
    /// </summary>
    public partial class DescriptionPage : Page
    {
        public ushort _atkType = 0;
        public ushort _element = 0;
        public float _skillDamage = 0;

        public DescriptionPage()
        {
            InitializeComponent();
        }
        public void postInitializer()
        {
            if (_atkType == 0)
            {
                ATTACK_SELFCAST_CHECKBOX.IsChecked = true;
            }
            else if (_atkType == 1)
            {
                ATTACK_MELEE_CHECKBOX.IsChecked = true;
            }
            else if (_atkType == 2)
            {
                ATTACK_RANGED_CHECKBOX.IsChecked = true;
            }
            else if (_atkType == 3)
            {
                ATTACK_AREA_CHECKBOX.IsChecked = true;
            }

            dmgSlider.Value = _skillDamage;

            if (_element == 0)
            {
                DAMAGE_PHYSICAL_CHECKBOX.IsChecked = true;
            }
            else if (_element == 1)
            {
                DAMAGE_RANGED_CHECKBOX.IsChecked = true;
            }
            else if (_element == 2)
            {
                DAMAGE_MAGICAL_CHECKBOX.IsChecked = true;
            }
            else if (_element == 3)
            {
                DAMAGE_FIRE_CHECKBOX.IsChecked = true;
            }
            else if (_element == 4)
            {
                DAMAGE_WATER_CHECKBOX.IsChecked = true;
            }
            else if (_element == 5)
            {
                DAMAGE_NATURE_CHECKBOX.IsChecked = true;
            }
        }

        private void SkillName_KeyUp(object sender, KeyEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            if (textBox != null)
            {
                if (e.Key == Key.Return)
                {
                    if (e.Key == Key.Enter)
                    {
                        textBox.MoveFocus(new TraversalRequest(FocusNavigationDirection.Next));
                    }
                }
                else if (e.Key == Key.Escape)
                {
                    textBox.Undo();
                    textBox.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
                }
            }
        }

        private void TextBox_KeyUp(object sender, KeyEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            if (textBox != null)
            {
                if (e.Key == Key.Return)
                {
                    if (e.Key == Key.Enter)
                    {
                        string txt = Dmg.Text;
                        decimal dml = 0;
                        bool succ = Decimal.TryParse(txt, out dml);
                        if (!succ)
                        {
                            Dmg.Undo();
                            if (String.IsNullOrWhiteSpace(Dmg.Text))
                            {
                                Dmg.Undo();
                            }
                            return;
                        }

                        dmgSlider.Value = (double)dml;
                        textBox.MoveFocus(new TraversalRequest(FocusNavigationDirection.Next));
                    }
                }
                else if (e.Key == Key.Escape)
                {
                    textBox.Undo();
                    textBox.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
                }
            }
        }

        private void AtkType_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton check = sender as RadioButton;
            if (check.Name == "ATTACK_SELFCAST_CHECKBOX")
            {
                _atkType = 0;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "ATTACK_MELEE_CHECKBOX")
            {
                _atkType = 1;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "ATTACK_RANGED_CHECKBOX")
            {
                _atkType = 2;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "ATTACK_AREA_CHECKBOX")
            {
                _atkType = 3;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }

            else { return; }
        }
        private void element_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton check = sender as RadioButton;
            if (check.Name == "DAMAGE_PHYSICAL_CHECKBOX")
            {
                _element = 0;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "DAMAGE_RANGED_CHECKBOX")
            {
                _element = 1;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "DAMAGE_MAGICAL_CHECKBOX")
            {
                _element = 2;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "DAMAGE_FIRE_CHECKBOX")
            {
                _element = 3;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "DAMAGE_WATER_CHECKBOX")
            {
                _element = 4;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "DAMAGE_NATURE_CHECKBOX")
            {
                _element = 5;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }

            else { return; }
        }

        private void dmgSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Slider s = sender as Slider;
            _skillDamage = (float)Math.Round(e.NewValue, 2);
            s.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
        }
    }
}
