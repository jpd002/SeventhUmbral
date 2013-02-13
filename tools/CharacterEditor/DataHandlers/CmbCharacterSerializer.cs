using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace CharacterEditor
{
    static class CmbCharacterSerializer
    {
        public static Character Load(Stream stream)
        {
            var character = new Character();
            var encryptedData = new byte[stream.Length];
            stream.Read(encryptedData, 0, encryptedData.Length);
            var characterData = encryptedData.Select(x => (byte)(x ^ 0x73)).ToArray();
            var characterDataString = UTF8Encoding.UTF8.GetString(characterData, 1, characterData.Length - 1);
            var rows = characterDataString.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            var characterProperties = new Dictionary<string, string>();
            foreach (var row in rows)
            {
                var cols = row.Split(new char[] { '\t', ',' }, StringSplitOptions.RemoveEmptyEntries);
                characterProperties[cols[0]] = cols[3];
            }
            character.Tribe = (Tribe)GetDictionaryIntValue(characterProperties, "rsc_tribe");
            character.Size          = GetDictionaryIntValue(characterProperties, "appearancetype_size");
            character.Voice         = GetDictionaryIntValue(characterProperties, "appearancetype_voice");
            character.Skin          = GetDictionaryIntValue(characterProperties, "appearancetype_skin");
            character.HairStyle     = GetDictionaryIntValue(characterProperties, "appearancetype_hairstyle");
            character.HairColor     = GetDictionaryIntValue(characterProperties, "appearancetype_haircolor");
            character.HairOption    = GetDictionaryIntValue(characterProperties, "appearancetype_hairoption2");
            character.EyeColor      = GetDictionaryIntValue(characterProperties, "appearancetype_eyecolor");
            character.FaceType      = GetDictionaryIntValue(characterProperties, "appearancetype_facetype");
            character.FaceBrow      = GetDictionaryIntValue(characterProperties, "appearancetype_facebrow");
            character.FaceEye       = GetDictionaryIntValue(characterProperties, "appearancetype_faceeye");
            character.FaceIris      = GetDictionaryIntValue(characterProperties, "appearancetype_faceiris");
            character.FaceNose      = GetDictionaryIntValue(characterProperties, "appearancetype_facenose");
            character.FaceMouth     = GetDictionaryIntValue(characterProperties, "appearancetype_facemouth");
            character.FaceJaw       = GetDictionaryIntValue(characterProperties, "appearancetype_facejaw_special");
            character.FaceCheek     = GetDictionaryIntValue(characterProperties, "appearancetype_facecheek");
            character.FaceOption1   = GetDictionaryIntValue(characterProperties, "appearancetype_faceoption1");
            character.FaceOption2   = GetDictionaryIntValue(characterProperties, "appearancetype_faceoption2");
            character.Guardian      = GetDictionaryIntValue(characterProperties, "guardian");
            character.BirthMonth    = GetDictionaryIntValue(characterProperties, "birth_month");
            character.BirthDay      = GetDictionaryIntValue(characterProperties, "birth_day");
            return character;
        }

        private static int GetDictionaryIntValue(Dictionary<string, string> dictionary, string key)
        {
            string value = dictionary[key];
            return int.Parse(value);
        }
    }
}
