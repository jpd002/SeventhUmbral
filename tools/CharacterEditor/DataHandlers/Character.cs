using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CharacterEditor
{
    public enum Tribe
    {
        Tribe_HyurMidlanderMale = 0,
        Tribe_HyurMidlanderFemale = 1,
        Tribe_HyurHighlanderMale = 3,
        Tribe_ElezenWildwoodMale = 4,
        Tribe_ElezenWildwoodFemale = 5,
        Tribe_ElezenDuskwightMale = 6,
        Tribe_ElezenDuskwightFemale = 7,
        Tribe_LalafellPlainsfolkMale = 8,
        Tribe_LalafellPlainsfolkFemale = 9,
        Tribe_LalafellDunesfolkMale = 10,
        Tribe_LalafellDunesfolkFemale = 11,
        Tribe_MiqoteSeekerOfTheSunFemale = 12,
        Tribe_MiqoteKeeperOfTheMoonFemale = 13,
        Tribe_RoegadynSeaWolfMale = 14,
        Tribe_RoegadynHellsguardMale = 15
    };

    public class Character
    {
        public Character()
        {
            Name = String.Empty;
            HeadGear = 0;
            BodyGear = 0x400;
            LegsGear = 0x400;
            HandsGear = 0x400;
            FeetGear = 0x400;
            Active = true;
        }

        public bool Active { get; set; }

        public string Name { get; set; }
        public Tribe Tribe { get; set; }
        public int Size { get; set; }
        public int Voice { get; set; }
        public int Skin { get; set; }
        public int HairStyle { get; set; }
        public int HairColor { get; set; }
        public int HairOption { get; set; }
        public int EyeColor { get; set; }
        public int FaceType { get; set; }
        public int FaceBrow { get; set; }
        public int FaceEye { get; set; }
        public int FaceIris { get; set; }
        public int FaceNose { get; set; }
        public int FaceMouth { get; set; }
        public int FaceJaw { get; set; }
        public int FaceCheek { get; set; }
        public int FaceOption1 { get; set; }
        public int FaceOption2 { get; set; }

        public int Guardian { get; set; }
        public int BirthMonth { get; set; }
        public int BirthDay { get; set; }

        public int HeadGear { get; set; }
        public int BodyGear { get; set; }
        public int LegsGear { get; set; }
        public int HandsGear { get; set; }
        public int FeetGear { get; set; }
    }
}
