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
    /// Interaction logic for BoonsPage.xaml
    /// </summary>
    public partial class BoonsPage : Page
    {
        public ushort _Boon;
        public float _effectValue;
        public float _rangeValue;
        public float _duration;

        public BoonsPage()
        {
            InitializeComponent();
        }
        public void postInitializer()
        {
            if (_Boon == 0)
            {
                BOON_NONE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 1)
            {
                BOON_DAMAGE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 2)
            {
                BOON_KNOCKBACK_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 3)
            {
                BOON_STUN_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 4)
            {
                BOON_ROOT_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 5)
            {
                BOON_PROTECTION_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 6)
            {
                BOON_PHYSICAL_RESISTANCE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 7)
            {
                BOON_MAGICAL_RESISTANCE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 8)
            {
                BOON_FIRE_RESISTANCE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 9)
            {
                BOON_WATER_RESISTANCE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 10)
            {
                BOON_NATURE_RESISTANCE_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 11)
            {
                BOON_CASTSPEED_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 12)
            {
                BOON_SWIFTNESS_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 13)
            {
                BOON_SLOW_CHECKBOX.IsChecked = true;
            }
            else if (_Boon == 14)
            {
                BOON_INVULNERABILITY_CHECKBOX.IsChecked = true;
            }

            BoonEffectSlider.Value = _effectValue;
            BoonRangeSlider.Value = _rangeValue;
            BoonDurationSlider.Value = _duration;
        }

            private void TextChanged(object sender, KeyEventArgs e)
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

        private void Text_lostFocus(object sender, RoutedEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            foreach (char c in textBox.Text)
            {
                if (!char.IsDigit(c))
                {
                    textBox.Undo();
                    break;
                }
            }
            
        }

        private void element_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton check = sender as RadioButton;
            if (check.Name == "BOON_NONE_CHECKBOX")
            {
                _Boon = 0;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_DAMAGE_CHECKBOX")
            {
                _Boon = 1;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_KNOCKBACK_CHECKBOX")
            {
                _Boon = 2;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_STUN_CHECKBOX")
            {
                _Boon = 3;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_ROOT_CHECKBOX")
            {
                _Boon = 4;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_PROTECTION_CHECKBOX")
            {
                _Boon = 5;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_PHYSICAL_RESISTANCE_CHECKBOX")
            {
                _Boon = 6;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_MAGICAL_RESISTANCE_CHECKBOX")
            {
                _Boon = 7;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_FIRE_RESISTANCE_CHECKBOX")
            {
                _Boon = 8;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_WATER_RESISTANCE_CHECKBOX")
            {
                _Boon = 9;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_NATURE_RESISTANCE_CHECKBOX")
            {
                _Boon = 10;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_CASTSPEED_CHECKBOX")
            {
                _Boon = 11;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_SWIFTNESS_CHECKBOX")
            {
                _Boon = 12;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_SLOW_CHECKBOX")
            {
                _Boon = 13;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BOON_INVULNERABILITY_CHECKBOX")
            {
                _Boon = 14;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }

            else { return; }
        }


        private void EffectSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Slider s = sender as Slider;
            _effectValue = (float)Math.Round(e.NewValue, 2);
            s.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
        }

        private void RangeSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Slider s = sender as Slider;
            _rangeValue = (float)Math.Round(e.NewValue, 2);
            s.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
        }

        private void DurationSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Slider s = sender as Slider;
            _duration = (float)Math.Round(e.NewValue, 2);
            s.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
        }
    }
}
