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
    /// Interaction logic for BanesPage.xaml
    /// </summary>
    public partial class BanesPage : Page
    {
        public ushort _Bane;
        public float _effectValue;
        public float _rangeValue;
        public float _duration;

        public BanesPage()
        {
            InitializeComponent();
        }
        public void postInitializer()
        {
            if (_Bane == 0)
            {
                BANE_NONE_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 1)
            {
                BANE_DAMAGE_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 2)
            {
                BANE_STUN_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 3)
            {
                BANE_ROOT_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 4)
            {
                BANE_BLOODLETTING_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 5)
            {
                BANE_UNCOVER_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 6)
            {
                BANE_PHYSICAL_WEAKNESS_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 7)
            {
                BANE_MAGICAL_WEAKNESS_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 8)
            {
                BANE_FIRE_WEAKNESS_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 9)
            {
                BANE_WATER_WEAKNESS_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 10)
            {
                BANE_NATURE_WEAKNESS_CHECKBOX.IsChecked = true;
            }
            else if (_Bane == 11)
            {
                BANE_SLOW_CHECKBOX.IsChecked = true;
            }


            BaneEffectSlider.Value = _effectValue;
            BaneRangeSlider.Value = _rangeValue;
            BaneDurationSlider.Value = _duration;
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
                _Bane = 0;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_DAMAGE_CHECKBOX")
            {
                _Bane = 1;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_STUN_CHECKBOX")
            {
                _Bane = 2;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_ROOT_CHECKBOX")
            {
                _Bane = 3;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_BLOODLETTING_CHECKBOX")
            {
                _Bane = 4;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_UNCOVER_CHECKBOX")
            {
                _Bane = 5;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_PHYSICAL_WEAKNESS_CHECKBOX")
            {
                _Bane = 6;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_MAGICAL_WEAKNESS_CHECKBOX")
            {
                _Bane = 7;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_FIRE_WEAKNESS_CHECKBOX")
            {
                _Bane = 8;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_WATER_WEAKNESS_CHECKBOX")
            {
                _Bane = 9;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_NATURE_WEAKNESS_CHECKBOX")
            {
                _Bane = 10;
                check.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous));
            }
            else if (check.Name == "BANE_SLOW_CHECKBOX")
            {
                _Bane = 11;
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
