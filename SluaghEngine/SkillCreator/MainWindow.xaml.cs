using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Shapes;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Collections.Generic;
using System.Windows.Media.Imaging;
using Microsoft.Win32;

namespace SkillCreator
{
   
    public partial class MainWindow : Window
    {
        public bool alreadySaved = false;
        public string fileName = "";
        public string filePath = "";

        public struct Skill
        {
            public string name;
            public ushort atkType;
            public ushort element;
            public ushort boon;
            public ushort bane;
            public ushort animation;
            public ushort particle;

            public float skillDamage;
            public float boonEffectValue;
            public float boonRange;
            public float boonDuration;
            public float baneEffectValue;
            public float baneRange;
            public float baneDuration;
        }
        public List<Skill> skills;

        public MainWindow()
        {
            InitializeComponent();
            WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;

            //this.Title

            skills = new List<Skill>();
        }

        private void NewDocument(object sender, RoutedEventArgs e)
        {
            if (alreadySaved == false)
            {
                MessageBoxResult res = MessageBox.Show("Do you want to save your current document?", "Create new document", MessageBoxButton.YesNoCancel, MessageBoxImage.Question);

                switch (res)
                {
                    case MessageBoxResult.Yes:
                        SaveFile(sender, e);
                        Menu_List.Items.Clear();
                        Doc_File.Clear();
                        this.Title = "MainWindow";
                        alreadySaved = true;
                        //message box containing fileName
                        break;
                    case MessageBoxResult.No:
                        Menu_List.Items.Clear();
                        fileName = "";
                        filePath = "";
                        Doc_File.Clear();
                        alreadySaved = true;
                        break;
                    case MessageBoxResult.Cancel:
                        break;
                }

            }
        }

        private void getFile(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openDialog = new OpenFileDialog();
            openDialog.Filter = "Skill Files (*.si)|*.si";
            openDialog.DefaultExt = "si";
            if (openDialog.ShowDialog() == true)
            {
                string tempFileName = System.IO.Path.GetFileNameWithoutExtension(openDialog.FileName);
                string tempFilePath = System.IO.Path.GetDirectoryName(openDialog.FileName);

                if (System.IO.Path.GetExtension(openDialog.FileName) == ".si")
                {
                    fileName    = tempFileName;
                    filePath    = tempFilePath;
                    Doc_File.Text   = fileName;
                    Menu_List.Items.Clear();
                    LoadFile();
                }
            }
        }
        private void LoadFile()
        {
            FileStream fsi = File.OpenRead(filePath + "\\" + fileName + ".si");
            FileStream fsa = File.OpenRead(filePath + "\\" + fileName + ".sa");
            BinaryReader bri = new BinaryReader(fsi);
            BinaryReader bra = new BinaryReader(fsa);
            skills.Clear();

            int itemCount;
            itemCount = bri.ReadInt32();
            for (int i = 0; i < itemCount; i++)
            {
                ASCIIEncoding asen = new ASCIIEncoding();
                Skill tempSkill = new Skill();

                uint strSize = bri.ReadUInt32();
                byte[] tempName = bri.ReadBytes((int)strSize);
                tempSkill.name = asen.GetString(tempName);

                tempSkill.atkType   = bri.ReadUInt16();
                tempSkill.element   = bri.ReadUInt16();
                tempSkill.boon      = bri.ReadUInt16();
                tempSkill.bane      = bri.ReadUInt16();
                tempSkill.animation = bri.ReadUInt16();
                tempSkill.particle  = bri.ReadUInt16();

                tempSkill.skillDamage       = bra.ReadSingle();
                tempSkill.boonEffectValue   = bra.ReadSingle();
                tempSkill.boonRange         = bra.ReadSingle();
                tempSkill.boonDuration      = bra.ReadSingle();
                tempSkill.baneEffectValue   = bra.ReadSingle();
                tempSkill.baneRange         = bra.ReadSingle();
                tempSkill.baneDuration      = bra.ReadSingle();

                skills.Add(tempSkill);
                Menu_List.Items.Add(tempSkill.name);
            }
        }

        private void SaveFile(object sender, RoutedEventArgs e)
        {
            if (alreadySaved == false)
            {
                SaveAsFile(sender, e);
            }
        }
        private void SaveAsFile(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveDialog = new SaveFileDialog();
            saveDialog.FileName = Doc_File.Text;
            saveDialog.Filter = "Skill Files (*.si)|*.si";
            saveDialog.DefaultExt = "si";
            if (saveDialog.ShowDialog() == true)
            {
                string tempString = saveDialog.FileName;
                fileName = System.IO.Path.GetFileNameWithoutExtension(saveDialog.FileName);
                filePath = System.IO.Path.GetDirectoryName(saveDialog.FileName);
                Doc_File.Text = fileName;
                CreateFile();
                alreadySaved = true;
            }
        }
        public void CreateFile()
        {
            int itemCount = Menu_List.Items.Count;

            if (itemCount <= 0)
            {
                return;
            }

            FileStream fsi = File.Create(filePath + "\\" + fileName + ".si");
            FileStream fsa = File.Create(filePath + "\\" + fileName + ".sa");
            BinaryWriter bwi = new BinaryWriter(fsi);
            BinaryWriter bwa = new BinaryWriter(fsa);

            bwi.Write(itemCount);
            for (int i = 0; i < itemCount; i++)
            {
                ASCIIEncoding asen = new ASCIIEncoding();
                Skill tempSkill = skills.ElementAt(i);

                byte[] tempName = asen.GetBytes(tempSkill.name);
                bwi.Write((uint)tempSkill.name.Length);
                bwi.Write(tempName);

                bwi.Write(tempSkill.atkType);
                bwi.Write(tempSkill.element);
                bwi.Write(tempSkill.boon);
                bwi.Write(tempSkill.bane);
                bwi.Write(tempSkill.animation);
                bwi.Write(tempSkill.particle);
            
                bwa.Write(tempSkill.skillDamage);
                bwa.Write(tempSkill.boonEffectValue);
                bwa.Write(tempSkill.boonRange);
                bwa.Write(tempSkill.boonDuration);
                bwa.Write(tempSkill.baneEffectValue);
                bwa.Write(tempSkill.baneRange);
                bwa.Write(tempSkill.baneDuration);
            }

            bwi.Close();
            bwa.Close();
            fsi.Close();
            fsa.Close();
        }

        private void Menu_List_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (Menu_List.Items.Count > 0)
            {
                if (Menu_List.SelectedIndex >= 0)
                {
                    alreadySaved = false;
                    var host = new AddSkillWindow(skills.ElementAt(Menu_List.SelectedIndex));
                    host.Owner = this;
                    host.ShowDialog();
                }
                else
                {
                    MessageBox.Show("No skill item was selected!", "🐷 Nothing Selected! 🐷", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }
            }
            else
            {
                MessageBox.Show("There are no items to load!", "Nothing there!", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
        }

        private void Remove_Skill_Click(object sender, RoutedEventArgs e)
        {
            if (Menu_List.Items.Count > 0)
            {
                if (Menu_List.SelectedIndex >= 0)
                {
                    alreadySaved = false;
                    skills.RemoveAt(Menu_List.SelectedIndex);
                    Menu_List.Items.RemoveAt(Menu_List.SelectedIndex);
                }
                else
                {
                    MessageBox.Show("No skill item was selected!", "🐷 Nothing Selected! 🐷", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }
            }
            else
            {
                MessageBox.Show("There are no items to load!", "Nothing there!", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
        }

        private void CreateSkill(object sender, RoutedEventArgs e)
        {
            alreadySaved = false;
            var host = new AddSkillWindow();
            host.Owner = this;
            host.ShowDialog();
        }

        private void EditSkill(object sender, RoutedEventArgs e)
        {
            if (Menu_List.Items.Count > 0)
            {
                if (Menu_List.SelectedIndex >= 0)
                {
                    alreadySaved = false;
                    var host = new AddSkillWindow(skills.ElementAt(Menu_List.SelectedIndex));
                    host.Owner = this;
                    host.ShowDialog();
                }
                else
                {
                    MessageBox.Show("No skill item was selected!", "🐷 Nothing Selected! 🐷", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }
            }
            else
            {
                MessageBox.Show("There are no items to load!", "Nothing there!", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult res = MessageBox.Show("Are you sure you want to save before you exit?", "Exit?", MessageBoxButton.YesNoCancel, MessageBoxImage.Asterisk);

            switch (res)
            {
                case MessageBoxResult.Yes:
                    SaveAsFile(sender, e);
                    if (alreadySaved == true)
                    {
                        this.Close();
                    }
                    break;

                case MessageBoxResult.No:
                    this.Close();
                    break;

                case MessageBoxResult.Cancel:
                    break;
            }
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            var host = new AboutPage();
            host.Owner = this;
            host.ShowDialog();
        }

        //################################ COMMANDS ####################################

        //New
        private void NewCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }
        private void NewCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            NewDocument(sender, e);
        }
   
        //Load
        private void LoadCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }
        private void LoadCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            getFile(sender, e);
        }

        //Save
        private void SaveCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }
        private void SaveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            SaveFile(sender, e);
        }

        //SaveAs
        private void SaveAsCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }
        private void SaveAsCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            SaveAsFile(sender, e);
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

        //Delete
        private void MenuDeleteCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }
        private void MenuDeleteCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (Menu_List.Items.Count > 0)
            {
                if (Menu_List.SelectedIndex >= 0)
                {
                    skills.RemoveAt(Menu_List.SelectedIndex);
                    Menu_List.Items.RemoveAt(Menu_List.SelectedIndex);
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
                        fileName = Doc_File.Text;
                        this.Title = "MainWindow - " + fileName;
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

    }

    public static class CustomCommands
    {
        public static readonly RoutedUICommand New = new RoutedUICommand
           (
               "New",
               "New",
               typeof(CustomCommands),
               new InputGestureCollection()
               {
                        new KeyGesture(Key.N, ModifierKeys.Control)
               }
           );

        public static readonly RoutedUICommand Load = new RoutedUICommand
           (
               "Load",
               "Load",
               typeof(CustomCommands),
               new InputGestureCollection()
               {
                        new KeyGesture(Key.O, ModifierKeys.Control)
               }
           );

        public static readonly RoutedUICommand Save = new RoutedUICommand
           (
               "Save",
               "Save",
               typeof(CustomCommands),
               new InputGestureCollection()
               {
                        new KeyGesture(Key.S, ModifierKeys.Control)
               }
           );

        public static readonly RoutedUICommand SaveAs = new RoutedUICommand
           (
               "SaveAs",
               "SaveAs",
               typeof(CustomCommands),
               new InputGestureCollection()
               {
                        new KeyGesture(Key.S, ModifierKeys.Control | ModifierKeys.Shift)
               }
           );

        public static readonly RoutedUICommand Exit = new RoutedUICommand
            (
                "Exit",
                "Exit",
                typeof(CustomCommands),
                new InputGestureCollection()
                {
                        new KeyGesture(Key.F4, ModifierKeys.Alt)
                }
            );

        public static readonly RoutedUICommand Delete = new RoutedUICommand
            (
                "Delete",
                "Delete",
                typeof(CustomCommands),
                new InputGestureCollection()
                {
                        new KeyGesture(Key.Delete)
                }
            );
    }

}