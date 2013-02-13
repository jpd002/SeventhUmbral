namespace CharacterEditor
{
    partial class CharacterEditForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.activeCheckBox = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.nameTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tribeCombo = new System.Windows.Forms.ComboBox();
            this.saveButton = new System.Windows.Forms.Button();
            this.importButton = new System.Windows.Forms.Button();
            this.exportButton = new System.Windows.Forms.Button();
            this.importOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.sizeTextBox = new System.Windows.Forms.TextBox();
            this.voiceTextBox = new System.Windows.Forms.TextBox();
            this.skinColorTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.hairStyleTextBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.faceTypeTextBox = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.eyeColorTextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.hairOptionTextBox = new System.Windows.Forms.TextBox();
            this.hairColorTextBox = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.faceNoseTextBox = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.faceIrisTextBox = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.faceEyeTextBox = new System.Windows.Forms.TextBox();
            this.faceBrowTextBox = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.faceMouthTextBox = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.faceJawTextBox = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.faceCheekTextBox = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.faceOption1TextBox = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.faceOption2TextBox = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.birthDayTextBox = new System.Windows.Forms.TextBox();
            this.label20 = new System.Windows.Forms.Label();
            this.birthMonthTextBox = new System.Windows.Forms.TextBox();
            this.label21 = new System.Windows.Forms.Label();
            this.guardianTextBox = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.legsGearTextBox = new System.Windows.Forms.TextBox();
            this.label23 = new System.Windows.Forms.Label();
            this.bodyGearTextBox = new System.Windows.Forms.TextBox();
            this.label24 = new System.Windows.Forms.Label();
            this.headGearTextBox = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.handsGearTextBox = new System.Windows.Forms.TextBox();
            this.label26 = new System.Windows.Forms.Label();
            this.feetGearTextBox = new System.Windows.Forms.TextBox();
            this.label27 = new System.Windows.Forms.Label();
            this.exportSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.SuspendLayout();
            // 
            // activeCheckBox
            // 
            this.activeCheckBox.AutoSize = true;
            this.activeCheckBox.Location = new System.Drawing.Point(12, 462);
            this.activeCheckBox.Name = "activeCheckBox";
            this.activeCheckBox.Size = new System.Drawing.Size(56, 17);
            this.activeCheckBox.TabIndex = 27;
            this.activeCheckBox.Text = "Active";
            this.activeCheckBox.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Name:";
            // 
            // nameTextBox
            // 
            this.nameTextBox.Location = new System.Drawing.Point(12, 25);
            this.nameTextBox.Name = "nameTextBox";
            this.nameTextBox.Size = new System.Drawing.Size(416, 20);
            this.nameTextBox.TabIndex = 0;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 48);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Tribe:";
            // 
            // tribeCombo
            // 
            this.tribeCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.tribeCombo.FormattingEnabled = true;
            this.tribeCombo.Location = new System.Drawing.Point(12, 64);
            this.tribeCombo.Name = "tribeCombo";
            this.tribeCombo.Size = new System.Drawing.Size(416, 21);
            this.tribeCombo.TabIndex = 1;
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(191, 456);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(75, 23);
            this.saveButton.TabIndex = 28;
            this.saveButton.Text = "Save";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // importButton
            // 
            this.importButton.Location = new System.Drawing.Point(272, 456);
            this.importButton.Name = "importButton";
            this.importButton.Size = new System.Drawing.Size(75, 23);
            this.importButton.TabIndex = 29;
            this.importButton.Text = "Import";
            this.importButton.UseVisualStyleBackColor = true;
            this.importButton.Click += new System.EventHandler(this.importButton_Click);
            // 
            // exportButton
            // 
            this.exportButton.Location = new System.Drawing.Point(353, 456);
            this.exportButton.Name = "exportButton";
            this.exportButton.Size = new System.Drawing.Size(75, 23);
            this.exportButton.TabIndex = 30;
            this.exportButton.Text = "Export";
            this.exportButton.UseVisualStyleBackColor = true;
            this.exportButton.Click += new System.EventHandler(this.exportButton_Click);
            // 
            // importOpenFileDialog
            // 
            this.importOpenFileDialog.Filter = "All Supported Files (*.cmb; *.xml)|*.cmb; *.xml";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 88);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Size:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(116, 88);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Voice:";
            // 
            // sizeTextBox
            // 
            this.sizeTextBox.Location = new System.Drawing.Point(12, 104);
            this.sizeTextBox.Name = "sizeTextBox";
            this.sizeTextBox.Size = new System.Drawing.Size(95, 20);
            this.sizeTextBox.TabIndex = 2;
            this.sizeTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // voiceTextBox
            // 
            this.voiceTextBox.Location = new System.Drawing.Point(119, 104);
            this.voiceTextBox.Name = "voiceTextBox";
            this.voiceTextBox.Size = new System.Drawing.Size(95, 20);
            this.voiceTextBox.TabIndex = 3;
            this.voiceTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // skinColorTextBox
            // 
            this.skinColorTextBox.Location = new System.Drawing.Point(226, 104);
            this.skinColorTextBox.Name = "skinColorTextBox";
            this.skinColorTextBox.Size = new System.Drawing.Size(95, 20);
            this.skinColorTextBox.TabIndex = 4;
            this.skinColorTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(223, 88);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(58, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Skin Color:";
            // 
            // hairStyleTextBox
            // 
            this.hairStyleTextBox.Location = new System.Drawing.Point(333, 104);
            this.hairStyleTextBox.Name = "hairStyleTextBox";
            this.hairStyleTextBox.Size = new System.Drawing.Size(95, 20);
            this.hairStyleTextBox.TabIndex = 5;
            this.hairStyleTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(330, 88);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(55, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Hair Style:";
            // 
            // faceTypeTextBox
            // 
            this.faceTypeTextBox.Location = new System.Drawing.Point(333, 143);
            this.faceTypeTextBox.Name = "faceTypeTextBox";
            this.faceTypeTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceTypeTextBox.TabIndex = 9;
            this.faceTypeTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(330, 127);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(61, 13);
            this.label7.TabIndex = 22;
            this.label7.Text = "Face Type:";
            // 
            // eyeColorTextBox
            // 
            this.eyeColorTextBox.Location = new System.Drawing.Point(226, 143);
            this.eyeColorTextBox.Name = "eyeColorTextBox";
            this.eyeColorTextBox.Size = new System.Drawing.Size(95, 20);
            this.eyeColorTextBox.TabIndex = 8;
            this.eyeColorTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(223, 127);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(55, 13);
            this.label8.TabIndex = 20;
            this.label8.Text = "Eye Color:";
            // 
            // hairOptionTextBox
            // 
            this.hairOptionTextBox.Location = new System.Drawing.Point(119, 143);
            this.hairOptionTextBox.Name = "hairOptionTextBox";
            this.hairOptionTextBox.Size = new System.Drawing.Size(95, 20);
            this.hairOptionTextBox.TabIndex = 7;
            this.hairOptionTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // hairColorTextBox
            // 
            this.hairColorTextBox.Location = new System.Drawing.Point(12, 143);
            this.hairColorTextBox.Name = "hairColorTextBox";
            this.hairColorTextBox.Size = new System.Drawing.Size(95, 20);
            this.hairColorTextBox.TabIndex = 6;
            this.hairColorTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(116, 127);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(63, 13);
            this.label9.TabIndex = 17;
            this.label9.Text = "Hair Option:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(9, 127);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(56, 13);
            this.label10.TabIndex = 16;
            this.label10.Text = "Hair Color:";
            // 
            // faceNoseTextBox
            // 
            this.faceNoseTextBox.Location = new System.Drawing.Point(333, 182);
            this.faceNoseTextBox.Name = "faceNoseTextBox";
            this.faceNoseTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceNoseTextBox.TabIndex = 13;
            this.faceNoseTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(330, 166);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(62, 13);
            this.label11.TabIndex = 30;
            this.label11.Text = "Face Nose:";
            // 
            // faceIrisTextBox
            // 
            this.faceIrisTextBox.Location = new System.Drawing.Point(226, 182);
            this.faceIrisTextBox.Name = "faceIrisTextBox";
            this.faceIrisTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceIrisTextBox.TabIndex = 12;
            this.faceIrisTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(223, 166);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(50, 13);
            this.label12.TabIndex = 28;
            this.label12.Text = "Face Iris:";
            // 
            // faceEyeTextBox
            // 
            this.faceEyeTextBox.Location = new System.Drawing.Point(119, 182);
            this.faceEyeTextBox.Name = "faceEyeTextBox";
            this.faceEyeTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceEyeTextBox.TabIndex = 11;
            this.faceEyeTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // faceBrowTextBox
            // 
            this.faceBrowTextBox.Location = new System.Drawing.Point(12, 182);
            this.faceBrowTextBox.Name = "faceBrowTextBox";
            this.faceBrowTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceBrowTextBox.TabIndex = 10;
            this.faceBrowTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(116, 166);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(55, 13);
            this.label13.TabIndex = 25;
            this.label13.Text = "Face Eye:";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(9, 166);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(61, 13);
            this.label14.TabIndex = 24;
            this.label14.Text = "Face Brow:";
            // 
            // faceMouthTextBox
            // 
            this.faceMouthTextBox.Location = new System.Drawing.Point(12, 221);
            this.faceMouthTextBox.Name = "faceMouthTextBox";
            this.faceMouthTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceMouthTextBox.TabIndex = 14;
            this.faceMouthTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(9, 205);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(67, 13);
            this.label15.TabIndex = 32;
            this.label15.Text = "Face Mouth:";
            // 
            // faceJawTextBox
            // 
            this.faceJawTextBox.Location = new System.Drawing.Point(119, 221);
            this.faceJawTextBox.Name = "faceJawTextBox";
            this.faceJawTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceJawTextBox.TabIndex = 15;
            this.faceJawTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(116, 205);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(56, 13);
            this.label16.TabIndex = 34;
            this.label16.Text = "Face Jaw:";
            // 
            // faceCheekTextBox
            // 
            this.faceCheekTextBox.Location = new System.Drawing.Point(226, 221);
            this.faceCheekTextBox.Name = "faceCheekTextBox";
            this.faceCheekTextBox.Size = new System.Drawing.Size(95, 20);
            this.faceCheekTextBox.TabIndex = 16;
            this.faceCheekTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(223, 205);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(68, 13);
            this.label17.TabIndex = 36;
            this.label17.Text = "Face Cheek:";
            // 
            // faceOption1TextBox
            // 
            this.faceOption1TextBox.Location = new System.Drawing.Point(333, 221);
            this.faceOption1TextBox.Name = "faceOption1TextBox";
            this.faceOption1TextBox.Size = new System.Drawing.Size(95, 20);
            this.faceOption1TextBox.TabIndex = 17;
            this.faceOption1TextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(330, 205);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(77, 13);
            this.label18.TabIndex = 38;
            this.label18.Text = "Face Option 1:";
            // 
            // faceOption2TextBox
            // 
            this.faceOption2TextBox.Location = new System.Drawing.Point(12, 260);
            this.faceOption2TextBox.Name = "faceOption2TextBox";
            this.faceOption2TextBox.Size = new System.Drawing.Size(95, 20);
            this.faceOption2TextBox.TabIndex = 18;
            this.faceOption2TextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(9, 244);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(77, 13);
            this.label19.TabIndex = 40;
            this.label19.Text = "Face Option 2:";
            // 
            // birthDayTextBox
            // 
            this.birthDayTextBox.Location = new System.Drawing.Point(226, 309);
            this.birthDayTextBox.Name = "birthDayTextBox";
            this.birthDayTextBox.Size = new System.Drawing.Size(95, 20);
            this.birthDayTextBox.TabIndex = 21;
            this.birthDayTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(223, 293);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(53, 13);
            this.label20.TabIndex = 46;
            this.label20.Text = "Birth Day:";
            // 
            // birthMonthTextBox
            // 
            this.birthMonthTextBox.Location = new System.Drawing.Point(119, 309);
            this.birthMonthTextBox.Name = "birthMonthTextBox";
            this.birthMonthTextBox.Size = new System.Drawing.Size(95, 20);
            this.birthMonthTextBox.TabIndex = 20;
            this.birthMonthTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(116, 293);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(64, 13);
            this.label21.TabIndex = 44;
            this.label21.Text = "Birth Month:";
            // 
            // guardianTextBox
            // 
            this.guardianTextBox.Location = new System.Drawing.Point(12, 309);
            this.guardianTextBox.Name = "guardianTextBox";
            this.guardianTextBox.Size = new System.Drawing.Size(95, 20);
            this.guardianTextBox.TabIndex = 19;
            this.guardianTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(12, 293);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(53, 13);
            this.label22.TabIndex = 42;
            this.label22.Text = "Guardian:";
            // 
            // legsGearTextBox
            // 
            this.legsGearTextBox.Location = new System.Drawing.Point(226, 366);
            this.legsGearTextBox.Name = "legsGearTextBox";
            this.legsGearTextBox.Size = new System.Drawing.Size(95, 20);
            this.legsGearTextBox.TabIndex = 24;
            this.legsGearTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(223, 350);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(59, 13);
            this.label23.TabIndex = 52;
            this.label23.Text = "Legs Gear:";
            // 
            // bodyGearTextBox
            // 
            this.bodyGearTextBox.Location = new System.Drawing.Point(119, 366);
            this.bodyGearTextBox.Name = "bodyGearTextBox";
            this.bodyGearTextBox.Size = new System.Drawing.Size(95, 20);
            this.bodyGearTextBox.TabIndex = 23;
            this.bodyGearTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(116, 350);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(60, 13);
            this.label24.TabIndex = 50;
            this.label24.Text = "Body Gear:";
            // 
            // headGearTextBox
            // 
            this.headGearTextBox.Location = new System.Drawing.Point(15, 366);
            this.headGearTextBox.Name = "headGearTextBox";
            this.headGearTextBox.Size = new System.Drawing.Size(95, 20);
            this.headGearTextBox.TabIndex = 22;
            this.headGearTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(12, 350);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(62, 13);
            this.label25.TabIndex = 48;
            this.label25.Text = "Head Gear:";
            // 
            // handsGearTextBox
            // 
            this.handsGearTextBox.Location = new System.Drawing.Point(333, 366);
            this.handsGearTextBox.Name = "handsGearTextBox";
            this.handsGearTextBox.Size = new System.Drawing.Size(95, 20);
            this.handsGearTextBox.TabIndex = 25;
            this.handsGearTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(330, 350);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(67, 13);
            this.label26.TabIndex = 54;
            this.label26.Text = "Hands Gear:";
            // 
            // feetGearTextBox
            // 
            this.feetGearTextBox.Location = new System.Drawing.Point(15, 405);
            this.feetGearTextBox.Name = "feetGearTextBox";
            this.feetGearTextBox.Size = new System.Drawing.Size(95, 20);
            this.feetGearTextBox.TabIndex = 26;
            this.feetGearTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericTextBox_KeyPress);
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Location = new System.Drawing.Point(12, 389);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(57, 13);
            this.label27.TabIndex = 56;
            this.label27.Text = "Feet Gear:";
            // 
            // exportSaveFileDialog
            // 
            this.exportSaveFileDialog.Filter = "Character Description Files (*.xml)|*.xml";
            // 
            // CharacterEditForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(440, 488);
            this.Controls.Add(this.feetGearTextBox);
            this.Controls.Add(this.label27);
            this.Controls.Add(this.handsGearTextBox);
            this.Controls.Add(this.label26);
            this.Controls.Add(this.legsGearTextBox);
            this.Controls.Add(this.label23);
            this.Controls.Add(this.bodyGearTextBox);
            this.Controls.Add(this.label24);
            this.Controls.Add(this.headGearTextBox);
            this.Controls.Add(this.label25);
            this.Controls.Add(this.birthDayTextBox);
            this.Controls.Add(this.label20);
            this.Controls.Add(this.birthMonthTextBox);
            this.Controls.Add(this.label21);
            this.Controls.Add(this.guardianTextBox);
            this.Controls.Add(this.label22);
            this.Controls.Add(this.faceOption2TextBox);
            this.Controls.Add(this.label19);
            this.Controls.Add(this.faceOption1TextBox);
            this.Controls.Add(this.label18);
            this.Controls.Add(this.faceCheekTextBox);
            this.Controls.Add(this.label17);
            this.Controls.Add(this.faceJawTextBox);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.faceMouthTextBox);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.faceNoseTextBox);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.faceIrisTextBox);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.faceEyeTextBox);
            this.Controls.Add(this.faceBrowTextBox);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.faceTypeTextBox);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.eyeColorTextBox);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.hairOptionTextBox);
            this.Controls.Add(this.hairColorTextBox);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.hairStyleTextBox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.skinColorTextBox);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.voiceTextBox);
            this.Controls.Add(this.sizeTextBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.exportButton);
            this.Controls.Add(this.importButton);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.tribeCombo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.nameTextBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.activeCheckBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CharacterEditForm";
            this.Text = "Character Info";
            this.Load += new System.EventHandler(this.CharacterEditForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox activeCheckBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox nameTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox tribeCombo;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.Button importButton;
        private System.Windows.Forms.Button exportButton;
        private System.Windows.Forms.OpenFileDialog importOpenFileDialog;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox sizeTextBox;
        private System.Windows.Forms.TextBox voiceTextBox;
        private System.Windows.Forms.TextBox skinColorTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox hairStyleTextBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox faceTypeTextBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox eyeColorTextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox hairOptionTextBox;
        private System.Windows.Forms.TextBox hairColorTextBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox faceNoseTextBox;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox faceIrisTextBox;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox faceEyeTextBox;
        private System.Windows.Forms.TextBox faceBrowTextBox;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox faceMouthTextBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox faceJawTextBox;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox faceCheekTextBox;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox faceOption1TextBox;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox faceOption2TextBox;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.TextBox birthDayTextBox;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.TextBox birthMonthTextBox;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.TextBox guardianTextBox;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.TextBox legsGearTextBox;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.TextBox bodyGearTextBox;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TextBox headGearTextBox;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.TextBox handsGearTextBox;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.TextBox feetGearTextBox;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.SaveFileDialog exportSaveFileDialog;

    }
}

