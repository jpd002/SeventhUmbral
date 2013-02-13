using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CharacterEditor
{
    public class TribeItem
    {
        public Tribe Tribe { get; set; }

        public override string ToString()
        {
            switch (Tribe)
            {
                case Tribe.Tribe_HyurMidlanderMale:
                    return "Hyur Midlander Male";
                case Tribe.Tribe_HyurMidlanderFemale:
                    return "Hyur Midlander Female";
                case Tribe.Tribe_HyurHighlanderMale:
                    return "Hyur Highlander Male";
                case Tribe.Tribe_ElezenWildwoodMale:
                    return "Elezen Wildwood Male";
                case Tribe.Tribe_ElezenWildwoodFemale:
                    return "Elezen Wildwood Female";
                case Tribe.Tribe_ElezenDuskwightMale:
                    return "Elezen Duskwight Male";
                case Tribe.Tribe_ElezenDuskwightFemale:
                    return "Elezen Duskwight Female";
                case Tribe.Tribe_LalafellPlainsfolkMale:
                    return "Lalafell Plainsfolk Male";
                case Tribe.Tribe_LalafellPlainsfolkFemale:
                    return "Lalafell Plainsfolk Female";
                case Tribe.Tribe_LalafellDunesfolkMale:
                    return "Lalafell Dunesfolk Male";
                case Tribe.Tribe_LalafellDunesfolkFemale:
                    return "Lalafell Dunesfolk Female";
                case Tribe.Tribe_MiqoteSeekerOfTheSunFemale:
                    return "Miqo'te Seeker of the Sun Female";
                case Tribe.Tribe_MiqoteKeeperOfTheMoonFemale:
                    return "Miqo'te Keeper of the Moon Female";
                case Tribe.Tribe_RoegadynSeaWolfMale:
                    return "Roegadyn Sea Wolf Male";
                case Tribe.Tribe_RoegadynHellsguardMale:
                    return "Roegadyn Hellsguard Male";
                default:
                    return "(Unknown)";
            }
        }
    };
}
