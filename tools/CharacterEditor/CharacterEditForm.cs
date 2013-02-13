using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CharacterEditor
{
    public partial class CharacterEditForm : Form
    {
        Character _character = new Character();
        List<TribeItem> _tribeItems = new List<TribeItem>();

        public CharacterEditForm()
        {
            InitializeComponent();
        }

        private static string GetServerCharacterFilePath()
        {
            var myDocumentsPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            var characterPath = Path.Combine(myDocumentsPath, "ffxivd_character.xml");
            return characterPath;
        }

        private void UpdateUiFromCharacter()
        {
            nameTextBox.Text = _character.Name;
            activeCheckBox.Checked = _character.Active;

            tribeCombo.SelectedItem = _tribeItems.Find(x => x.Tribe == _character.Tribe);

            sizeTextBox.Text = _character.Size.ToString();
            voiceTextBox.Text = _character.Voice.ToString();
            skinColorTextBox.Text = _character.Skin.ToString();
            hairStyleTextBox.Text = _character.HairStyle.ToString();
            hairColorTextBox.Text = _character.HairColor.ToString();
            hairOptionTextBox.Text = _character.HairOption.ToString();
            eyeColorTextBox.Text = _character.EyeColor.ToString();
            faceTypeTextBox.Text = _character.FaceType.ToString();
            faceBrowTextBox.Text = _character.FaceBrow.ToString();
            faceEyeTextBox.Text = _character.FaceEye.ToString();
            faceIrisTextBox.Text = _character.FaceIris.ToString();
            faceNoseTextBox.Text = _character.FaceNose.ToString();
            faceMouthTextBox.Text = _character.FaceMouth.ToString();
            faceJawTextBox.Text = _character.FaceJaw.ToString();
            faceCheekTextBox.Text = _character.FaceCheek.ToString();
            faceOption1TextBox.Text = _character.FaceOption1.ToString();
            faceOption2TextBox.Text = _character.FaceOption2.ToString();
            guardianTextBox.Text = _character.Guardian.ToString();
            birthMonthTextBox.Text = _character.BirthMonth.ToString();
            birthDayTextBox.Text = _character.BirthDay.ToString();

            headGearTextBox.Text = _character.HeadGear.ToString();
            bodyGearTextBox.Text = _character.BodyGear.ToString();
            legsGearTextBox.Text = _character.LegsGear.ToString();
            handsGearTextBox.Text = _character.HandsGear.ToString();
            feetGearTextBox.Text = _character.FeetGear.ToString();
        }

        private void UpdateCharacterFromUi()
        {
            var character = new Character();

            character.Active = activeCheckBox.Checked;
            character.Name = nameTextBox.Text;
            character.Tribe = ((TribeItem)tribeCombo.SelectedItem).Tribe;
            character.Size = int.Parse(sizeTextBox.Text);
            character.Voice = int.Parse(voiceTextBox.Text);
            character.Skin = int.Parse(skinColorTextBox.Text);
            character.HairStyle = int.Parse(hairStyleTextBox.Text);
            character.HairColor = int.Parse(hairColorTextBox.Text);
            character.HairOption = int.Parse(hairOptionTextBox.Text);
            character.EyeColor = int.Parse(eyeColorTextBox.Text);
            character.FaceType = int.Parse(faceTypeTextBox.Text);
            character.FaceBrow = int.Parse(faceBrowTextBox.Text);
            character.FaceEye = int.Parse(faceEyeTextBox.Text);
            character.FaceIris = int.Parse(faceIrisTextBox.Text);
            character.FaceNose = int.Parse(faceNoseTextBox.Text);
            character.FaceMouth = int.Parse(faceMouthTextBox.Text);
            character.FaceJaw = int.Parse(faceJawTextBox.Text);
            character.FaceCheek = int.Parse(faceCheekTextBox.Text);
            character.FaceOption1 = int.Parse(faceOption1TextBox.Text);
            character.FaceOption2 = int.Parse(faceOption2TextBox.Text);
            character.Guardian = int.Parse(guardianTextBox.Text);
            character.BirthMonth = int.Parse(birthMonthTextBox.Text);
            character.BirthDay = int.Parse(birthDayTextBox.Text);
            character.HeadGear = int.Parse(headGearTextBox.Text);
            character.BodyGear = int.Parse(bodyGearTextBox.Text);
            character.LegsGear = int.Parse(legsGearTextBox.Text);
            character.HandsGear = int.Parse(handsGearTextBox.Text);
            character.FeetGear = int.Parse(feetGearTextBox.Text);

            _character = character;
        }

        #region Event Handlers

        private void numericTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar)
                && !char.IsDigit(e.KeyChar))
            {
                e.Handled = true;
            }
        }

        private void CharacterEditForm_Load(object sender, EventArgs e)
        {
            foreach (Tribe tribe in System.Enum.GetValues(typeof(Tribe)))
            {
                _tribeItems.Add(new TribeItem { Tribe = tribe });
            }
            tribeCombo.DataSource = _tribeItems;

            try
            {
                using (var stream = File.OpenRead(GetServerCharacterFilePath()))
                {
                    _character = XmlCharacterSerializer.Load(stream);
                }
            }
            catch (Exception exception)
            {

            }

            UpdateUiFromCharacter();
        }

        private void importButton_Click(object sender, EventArgs e)
        {
            var myDocumentsPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            var templatesPath = Path.Combine(myDocumentsPath, @"My Games\FINAL FANTASY XIV\user\00000000");
            importOpenFileDialog.InitialDirectory = templatesPath;
            if (importOpenFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                var fileExtension = Path.GetExtension(importOpenFileDialog.FileName);
                switch (fileExtension)
                {
                    case ".cmb":
                        using (var fileStream = File.OpenRead(importOpenFileDialog.FileName))
                        {
                            _character = CmbCharacterSerializer.Load(fileStream);
                        }
                        break;
                    case ".xml":
                        using (var fileStream = File.OpenRead(importOpenFileDialog.FileName))
                        {
                            _character = XmlCharacterSerializer.Load(fileStream);
                        }
                        break;
                }

                UpdateUiFromCharacter();
            }
        }

        private void exportButton_Click(object sender, EventArgs e)
        {
            var myDocumentsPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            var templatesPath = Path.Combine(myDocumentsPath, @"My Games\FINAL FANTASY XIV\user\00000000");
            exportSaveFileDialog.InitialDirectory = templatesPath;
            exportSaveFileDialog.FileName = String.Empty;
            if (exportSaveFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                UpdateCharacterFromUi();
                using (var fileStream = File.Open(exportSaveFileDialog.FileName, FileMode.Create, FileAccess.Write))
                {
                    XmlCharacterSerializer.Save(fileStream, _character);
                }
            }
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            UpdateCharacterFromUi();
            using (var fileStream = File.Open(GetServerCharacterFilePath(), FileMode.Create, FileAccess.Write))
            {
                XmlCharacterSerializer.Save(fileStream, _character);
            }
        }

        #endregion
    }
}
