using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CharacterEditor
{
    static class XmlCharacterSerializer
    {
        public static Character Load(Stream stream)
        {
            var character = new Character();
            using (var reader = XmlReader.Create(stream))
            {
                var document = XDocument.Load(reader);
                var characterElement = document.Element("Character");
                character.Active = bool.Parse(characterElement.Attribute("Active").Value);
                character.Name = characterElement.Attribute("Name").Value;
                character.Tribe = (Tribe)int.Parse(characterElement.Attribute("Tribe").Value);
                character.Size = int.Parse(characterElement.Attribute("Size").Value);
                character.Voice = int.Parse(characterElement.Attribute("Voice").Value);
                character.Skin = int.Parse(characterElement.Attribute("Skin").Value);
                character.HairStyle = int.Parse(characterElement.Attribute("HairStyle").Value);
                character.HairColor = int.Parse(characterElement.Attribute("HairColor").Value);
                character.HairOption = int.Parse(characterElement.Attribute("HairOption").Value);
                character.EyeColor = int.Parse(characterElement.Attribute("EyeColor").Value);
                character.FaceType = int.Parse(characterElement.Attribute("FaceType").Value);
                character.FaceBrow = int.Parse(characterElement.Attribute("FaceBrow").Value);
                character.FaceEye = int.Parse(characterElement.Attribute("FaceEye").Value);
                character.FaceIris = int.Parse(characterElement.Attribute("FaceIris").Value);
                character.FaceNose = int.Parse(characterElement.Attribute("FaceNose").Value);
                character.FaceMouth = int.Parse(characterElement.Attribute("FaceMouth").Value);
                character.FaceJaw = int.Parse(characterElement.Attribute("FaceJaw").Value);
                character.FaceCheek = int.Parse(characterElement.Attribute("FaceCheek").Value);
                character.FaceOption1 = int.Parse(characterElement.Attribute("FaceOption1").Value);
                character.FaceOption2 = int.Parse(characterElement.Attribute("FaceOption2").Value);
                character.Guardian = int.Parse(characterElement.Attribute("Guardian").Value);
                character.BirthMonth = int.Parse(characterElement.Attribute("BirthMonth").Value);
                character.BirthDay = int.Parse(characterElement.Attribute("BirthDay").Value);
                character.HeadGear = int.Parse(characterElement.Attribute("HeadGear").Value);
                character.BodyGear = int.Parse(characterElement.Attribute("BodyGear").Value);
                character.LegsGear = int.Parse(characterElement.Attribute("LegsGear").Value);
                character.HandsGear = int.Parse(characterElement.Attribute("HandsGear").Value);
                character.FeetGear = int.Parse(characterElement.Attribute("FeetGear").Value);
            }
            return character;
        }

        public static void Save(Stream stream, Character character)
        {
            var document = new XElement("Character",
                            new XAttribute("Active", character.Active),
                            new XAttribute("Name", character.Name),
                            new XAttribute("Tribe", (int)character.Tribe),
                            new XAttribute("Size", character.Size),
                            new XAttribute("Voice", character.Voice),
                            new XAttribute("Skin", character.Skin),
                            new XAttribute("HairStyle", character.HairStyle),
                            new XAttribute("HairColor", character.HairColor),
                            new XAttribute("HairOption", character.HairOption),
                            new XAttribute("EyeColor", character.EyeColor),
                            new XAttribute("FaceType", character.FaceType),
                            new XAttribute("FaceBrow", character.FaceBrow),
                            new XAttribute("FaceEye", character.FaceEye),
                            new XAttribute("FaceIris", character.FaceIris),
                            new XAttribute("FaceNose", character.FaceNose),
                            new XAttribute("FaceMouth", character.FaceMouth),
                            new XAttribute("FaceJaw", character.FaceJaw),
                            new XAttribute("FaceCheek", character.FaceCheek),
                            new XAttribute("FaceOption1", character.FaceOption1),
                            new XAttribute("FaceOption2", character.FaceOption2),
                            new XAttribute("Guardian", character.Guardian),
                            new XAttribute("BirthMonth", character.BirthMonth),
                            new XAttribute("BirthDay", character.BirthDay),
                            new XAttribute("HeadGear", character.HeadGear),
                            new XAttribute("BodyGear", character.BodyGear),
                            new XAttribute("LegsGear", character.LegsGear),
                            new XAttribute("HandsGear", character.HandsGear),
                            new XAttribute("FeetGear", character.FeetGear));

            var writerSettings = new XmlWriterSettings();
            writerSettings.NewLineOnAttributes = true;
            writerSettings.Indent = true;

            using (var writer = XmlWriter.Create(stream, writerSettings))
            {
                document.WriteTo(writer);
            }
        }
    }
}
