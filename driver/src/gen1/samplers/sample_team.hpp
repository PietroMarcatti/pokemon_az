#include <vector>
#include <string>
#include <unordered_map>
#include "gen1battle.hpp"
#include <random>

namespace pkmndriver::Gen1 {
    struct PokemonConfig {
        Species species;
        uint8_t level;
        std::vector<std::pair<Move, float>> moves;
        std::array<uint8_t, 5> EV;
        std::array<uint8_t, 5> IV;

        PokemonConfig(Species s, uint8_t l, std::vector<std::pair<Move,float>> m, std::array<uint8_t, 5> e = { { 63, 63, 63, 63, 63 } }, std::array<uint8_t, 5> i = { { 15, 2, 15, 15, 15 } }):
        species(s), level(l),moves(m),EV(e),IV(i){}
    };
    constexpr std::array<uint8_t, 5> zero_atk_evs = { 63, 0, 63, 63, 63 };
    constexpr std::array<uint8_t, 5> two_atk_ivs = { 15, 2, 15, 15, 15 };

    const std::vector<PokemonConfig> initializeConfigs() {
        return {
            PokemonConfig(Abra,84,{{Psychic,1.0f}, {SeismicToss,1.0f}, {ThunderWave,1.0f}, {Reflect,0.3378f}, {Counter,0.3338f}, {Substitute,0.3285f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Aerodactyl,75,{{DoubleEdge,1.0f}, {FireBlast,1.0f}, {HyperBeam,1.0f}, {SkyAttack,0.6676f}, {Agility,0.3324f}}),
            PokemonConfig(Alakazam,68,{{Psychic,1.0f}, {Recover,1.0f}, {ThunderWave,1.0f}, {Reflect,0.4003f}, {SeismicToss,0.3973f}, {Counter,0.2024f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Arbok,78,{{Earthquake,1.0f}, {Glare,1.0f}, {HyperBeam,1.0f}, {RockSlide,0.672f}, {BodySlam,0.328f}}),
            PokemonConfig(Arcanine,75,{{BodySlam,1.0f}, {FireBlast,1.0f}, {HyperBeam,1.0f}, {Flamethrower,0.337f}, {Agility,0.3202f}, {Rest,0.1722f}, {Reflect,0.1707f}}),
            PokemonConfig(Articuno,70,{{Blizzard,1.0f}, {IceBeam,0.6666f}, {Reflect,0.6594f}, {Agility,0.5024f}, {HyperBeam,0.5024f}, {Rest,0.4976f}, {Mimic,0.1716f}}),
            PokemonConfig(Beedrill,81,{{HyperBeam,1.0f}, {SwordsDance,1.0f}, {Twineedle,1.0f}, {Agility,0.6662f}, {MegaDrain,0.3338f}}),
            PokemonConfig(Bellsprout,88,{{DoubleEdge,1.0f}, {RazorLeaf,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.6599f}, {SwordsDance,0.3401f}}),
            PokemonConfig(Blastoise,75,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Surf,0.6653f}, {Earthquake,0.5111f}, {Rest,0.4889f}, {HydroPump,0.3347f}}),
            PokemonConfig(Bulbasaur,89,{{BodySlam,1.0f}, {RazorLeaf,1.0f}, {SleepPowder,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Butterfree,77,{{Psychic,1.0f}, {SleepPowder,1.0f}, {StunSpore,1.0f}, {MegaDrain,0.2563f}, {HyperBeam,0.2533f}, {Substitute,0.2474f}, {DoubleEdge,0.2431f}}),
            PokemonConfig(Chansey,68,{{IceBeam,1.0f}, {SoftBoiled,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,0.4293f}, {Counter,0.1466f}, {SeismicToss,0.1451f}, {Reflect,0.1408f}, {Sing,0.1382f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Charizard,74,{{Earthquake,1.0f}, {FireBlast,1.0f}, {BodySlam,0.505f}, {Slash,0.505f}, {HyperBeam,0.495f}, {SwordsDance,0.495f}}),
            PokemonConfig(Charmander,90,{{BodySlam,1.0f}, {FireBlast,1.0f}, {Submission,0.5027f}, {SwordsDance,0.5027f}, {SeismicToss,0.3922f}, {Slash,0.3768f}, {Counter,0.2257f}}),
            PokemonConfig(Charmeleon,81,{{BodySlam,1.0f}, {FireBlast,1.0f}, {Submission,0.491f}, {SwordsDance,0.491f}, {Slash,0.3903f}, {SeismicToss,0.3879f}, {Counter,0.2398f}}),
            PokemonConfig(Clefable,74,{{Blizzard,1.0f}, {ThunderWave,0.804f}, {BodySlam,0.8024f}, {Thunderbolt,0.5405f}, {HyperBeam,0.288f}, {Sing,0.2795f}, {Psychic,0.1438f}, {Counter,0.1418f}}),
            PokemonConfig(Clefairy,88,{{Blizzard,1.0f}, {ThunderWave,1.0f}, {BodySlam,0.6024f}, {SeismicToss,0.3677f}, {Thunderbolt,0.3558f}, {Sing,0.335f}, {Psychic,0.1748f}, {Counter,0.1644f}}),
            PokemonConfig(Cloyster,70,{{Blizzard,1.0f}, {Explosion,1.0f}, {Surf,1.0f}, {HyperBeam,0.6703f}, {DoubleEdge,0.3297f}}),
            PokemonConfig(Cubone,89,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Earthquake,1.0f}, {SeismicToss,1.0f}}),
            PokemonConfig(Dewgong,74,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Surf,1.0f}, {Rest,0.7474f}, {HyperBeam,0.2526f}}),
            PokemonConfig(Diglett,86,{{Earthquake,1.0f}, {RockSlide,1.0f}, {Slash,1.0f}, {BodySlam,0.5027f}, {Substitute,0.4973f}}),
            /*PokemonConfig(Ditto,100,{{Transform,1.0f}}),*/
            PokemonConfig(Dodrio,73,{{Agility,1.0f}, {BodySlam,1.0f}, {DrillPeck,1.0f}, {HyperBeam,1.0f}}),
            PokemonConfig(Doduo,87,{{Agility,1.0f}, {BodySlam,1.0f}, {DoubleEdge,1.0f}, {DrillPeck,1.0f}}),
            PokemonConfig(Dragonair,80,{{Blizzard,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,0.8355f}, {HyperBeam,0.5899f}, {BodySlam,0.5746f}}),
            PokemonConfig(Dragonite,74,{{Blizzard,1.0f}, {ThunderWave,0.8943f}, {Thunderbolt,0.7153f}, {BodySlam,0.7006f}, {HyperBeam,0.6898f}}),
            PokemonConfig(Dratini,89,{{Blizzard,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,0.8329f}, {HyperBeam,0.5911f}, {BodySlam,0.5761f}}),
            PokemonConfig(Drowzee,84,{{Hypnosis,1.0f}, {Psychic,1.0f}, {ThunderWave,1.0f}, {SeismicToss,0.4889f}, {Counter,0.2618f}, {Rest,0.2493f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Dugtrio,73,{{Earthquake,1.0f}, {RockSlide,1.0f}, {Slash,1.0f}, {Substitute,0.5043f}, {BodySlam,0.4957f}}),
            PokemonConfig(Eevee,88,{{BodySlam,1.0f}, {QuickAttack,0.7727f}, {DoubleEdge,0.7587f}, {TailWhip,0.5007f}, {SandAttack,0.4993f}, {Reflect,0.4686f}}),
            PokemonConfig(Ekans,90,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Glare,1.0f}, {RockSlide,1.0f}}),
            PokemonConfig(Electabuzz,74,{{Psychic,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {SeismicToss,0.7256f}, {HyperBeam,0.2744f}}),
            PokemonConfig(Electrode,76,{{Explosion,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {HyperBeam,0.3967f}, {Thunder,0.3953f}, {TakeDown,0.2081f}}),
            PokemonConfig(Exeggcute,84,{{Explosion,1.0f}, {Psychic,1.0f}, {SleepPowder,1.0f}, {StunSpore,1.0f}}),
            PokemonConfig(Exeggutor,68,{{Explosion,1.0f}, {Psychic,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.5035f}, {HyperBeam,0.1683f}, {MegaDrain,0.1646f}, {DoubleEdge,0.1636f}}),
            PokemonConfig(Farfetchd,78,{{Agility,1.0f}, {BodySlam,1.0f}, {Slash,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Fearow,75,{{Agility,1.0f}, {DoubleEdge,1.0f}, {DrillPeck,1.0f}, {HyperBeam,1.0f}}),
            PokemonConfig(Flareon,76,{{BodySlam,1.0f}, {FireBlast,1.0f}, {HyperBeam,1.0f}, {QuickAttack,1.0f}}),
            PokemonConfig(Gastly,83,{{Hypnosis,1.0f}, {Thunderbolt,1.0f}, {Explosion,0.6257f}, {Psychic,0.6217f}, {MegaDrain,0.3806f}, {NightShade,0.372f}}),
            PokemonConfig(Gengar,68,{{Hypnosis,1.0f}, {Thunderbolt,1.0f}, {Psychic,0.6465f}, {Explosion,0.6211f}, {NightShade,0.3875f}, {MegaDrain,0.3449f}}),
            PokemonConfig(Geodude,88,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Explosion,1.0f}, {RockSlide,1.0f}}),
            PokemonConfig(Gloom,82,{{DoubleEdge,1.0f}, {MegaDrain,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.6677f}, {SwordsDance,0.3323f}}),
            PokemonConfig(Golbat,78,{{ConfuseRay,1.0f}, {DoubleEdge,1.0f}, {HyperBeam,1.0f}, {MegaDrain,1.0f}}),
            PokemonConfig(Goldeen,88,{{Agility,1.0f}, {Blizzard,1.0f}, {DoubleEdge,1.0f}, {Surf,1.0f}}),
            PokemonConfig(Golduck,75,{{Amnesia,1.0f}, {Blizzard,1.0f}, {Surf,1.0f}, {Rest,0.3893f}, {BodySlam,0.2065f}, {HydroPump,0.2022f}, {SeismicToss,0.2019f}}),
            PokemonConfig(Golem,71,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Explosion,1.0f}, {RockSlide,1.0f}}),
            PokemonConfig(Graveler,80,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Explosion,1.0f}, {RockSlide,1.0f}}),
            PokemonConfig(Grimer,90,{{BodySlam,1.0f}, {Explosion,1.0f}, {Sludge,0.7416f}, {FireBlast,0.61f}, {MegaDrain,0.3263f}, {Thunderbolt,0.3221f}}),
            PokemonConfig(Growlithe,89,{{Agility,1.0f}, {BodySlam,1.0f}, {FireBlast,1.0f}, {Flamethrower,0.5166f}, {Reflect,0.4834f}}),
            PokemonConfig(Gyarados,74,{{BodySlam,0.898f}, {Thunderbolt,0.717f}, {Blizzard,0.6949f}, {HyperBeam,0.6902f}, {Surf,0.6702f}, {HydroPump,0.3298f}}),
            PokemonConfig(Haunter,74,{{Hypnosis,1.0f}, {Thunderbolt,1.0f}, {Psychic,0.6361f}, {Explosion,0.6284f}, {NightShade,0.3721f}, {MegaDrain,0.3634f}}),
            PokemonConfig(Hitmonchan,80,{{BodySlam,1.0f}, {SeismicToss,1.0f}, {Submission,1.0f}, {Agility,0.4134f}, {Counter,0.3852f}, {MegaKick,0.2014f}}),
            PokemonConfig(Hitmonlee,78,{{BodySlam,1.0f}, {HighJumpKick, 1.0f}, {SeismicToss,1.0f}, {Counter,0.3959f}, {RollingKick,0.2052f}, {Meditate,0.2011f}, {MegaKick,0.1978f}}),
            PokemonConfig(Horsea,88,{{Agility,1.0f}, {Blizzard,1.0f}, {Surf,1.0f}, {HydroPump,0.3381f}, {Smokescreen,0.3316f}, {DoubleEdge,0.3303f}}),
            PokemonConfig(Hypno,72,{{Hypnosis,1.0f}, {Psychic,1.0f}, {ThunderWave,1.0f}, {SeismicToss,0.4061f}, {Rest,0.3935f}, {Counter,0.2003f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Ivysaur,80,{{BodySlam,1.0f}, {RazorLeaf,1.0f}, {SleepPowder,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Jigglypuff,89,{{SeismicToss,1.0f}, {ThunderWave,1.0f}, {BodySlam,0.6738f}, {Blizzard,0.6579f}, {Sing,0.3438f}, {Counter,0.3245f}}),
            PokemonConfig(Jolteon,69,{{BodySlam,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {PinMissile,0.4023f}, {Agility,0.3965f}, {DoubleKick,0.2013f}}),
            PokemonConfig(Jynx,68,{{Blizzard,1.0f}, {LovelyKiss,1.0f}, {Psychic,1.0f}, {Counter,0.4046f}, {BodySlam,0.1992f}, {SeismicToss,0.1982f}, {Substitute,0.198f}}),
            PokemonConfig(Kabuto,88,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Slash,1.0f}, {Surf,0.6462f}, {HydroPump,0.3538f}}),
            PokemonConfig(Kabutops,75,{{HyperBeam,1.0f}, {Surf,1.0f}, {SwordsDance,1.0f}, {Slash,0.509f}, {BodySlam,0.491f}}),
            PokemonConfig(Kadabra,74,{{Psychic,1.0f}, {Recover,1.0f}, {ThunderWave,1.0f}, {Reflect,0.4045f}, {SeismicToss,0.3933f}, {Counter,0.2022f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Kangaskhan,73,{{BodySlam,1.0f}, {Earthquake,1.0f}, {HyperBeam,1.0f}, {RockSlide,0.495f}, {Surf,0.2584f}, {Counter,0.2466f}}),
            PokemonConfig(Kingler,76,{{BodySlam,1.0f}, {Crabhammer,1.0f}, {HyperBeam,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Koffing,90,{{Explosion,1.0f}, {FireBlast,1.0f}, {Sludge,1.0f}, {Thunderbolt,1.0f}}),
            PokemonConfig(Krabby,89,{{BodySlam,1.0f}, {Crabhammer,1.0f}, {SwordsDance,1.0f}, {Blizzard,0.7568f}, {Stomp,0.2432f}}),
            PokemonConfig(Lapras,69,{{Blizzard,1.0f}, {Thunderbolt,1.0f}, {BodySlam,0.5062f}, {Rest,0.5007f}, {Surf,0.4979f}, {Sing,0.4953f}}),
            PokemonConfig(Lickitung,78,{{BodySlam,1.0f}, {HyperBeam,1.0f}, {SwordsDance,1.0f}, {Earthquake,0.7468f}, {Blizzard,0.2532f}}),
            PokemonConfig(Machamp,76,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Submission,1.0f}, {RockSlide,0.4039f}, {HyperBeam,0.3922f}, {Counter,0.2039f}}),
            PokemonConfig(Machoke,81,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Submission,1.0f}, {RockSlide,0.6656f}, {Counter,0.3344f}}),
            PokemonConfig(Machop,89,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Submission,1.0f}, {RockSlide,0.6663f}, {Counter,0.3337f}}),
            PokemonConfig(Magmar,76,{{BodySlam,1.0f}, {ConfuseRay,1.0f}, {FireBlast,1.0f}, {SeismicToss,0.3435f}, {Psychic,0.3291f}, {HyperBeam,0.3274f}}),
            PokemonConfig(Magnemite,88,{{Thunder,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {DoubleEdge,0.5101f}, {Mimic,0.2449f}, {Rest,0.2449f}}),
            PokemonConfig(Magneton,76,{{Thunder,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {HyperBeam,0.4088f}, {DoubleEdge,0.2075f}, {Mimic,0.192f}, {Rest,0.1917f}}),
            PokemonConfig(Mankey,89,{{BodySlam,1.0f}, {RockSlide,1.0f}, {Submission,1.0f}, {MegaKick,0.3484f}, {LowKick,0.3277f}, {Counter,0.3239f}}),
            PokemonConfig(Marowak,79,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Earthquake,1.0f}, {SeismicToss,1.0f}}),
            PokemonConfig(Meowth,85,{{BodySlam,1.0f}, {BubbleBeam,1.0f}, {Slash,1.0f}, {Thunderbolt,1.0f}}),
            PokemonConfig(Mew,64,{{Psychic,1.0f}, {SoftBoiled,1.0f}, {ThunderWave,1.0f}, {Blizzard,0.3376f}, {Explosion,0.3361f}, {Thunderbolt,0.1708f}, {Earthquake,0.1555f}}),
            PokemonConfig(Mewtwo,60,{{Amnesia,1.0f}, {Psychic,1.0f}, {Recover,1.0f}, {ThunderWave,0.4967f}, {Blizzard,0.2545f}, {Thunderbolt,0.2488f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Moltres,73,{{Agility,1.0f}, {FireBlast,1.0f}, {HyperBeam,1.0f}, {DoubleEdge,0.7412f}, {Reflect,0.2588f}}),
            PokemonConfig(Mr_Mime,75,{{Psychic,1.0f}, {SeismicToss,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Muk,76,{{BodySlam,1.0f}, {Explosion,1.0f}, {Sludge,0.6036f}, {FireBlast,0.4577f}, {MegaDrain,0.4537f}, {HyperBeam,0.2455f}, {Thunderbolt,0.2395f}}),
            PokemonConfig(Nidoking,74,{{Blizzard,1.0f}, {Earthquake,1.0f}, {Thunderbolt,0.6736f}, {BodySlam,0.659f}, {Substitute,0.341f}, {RockSlide,0.3264f}}),
            PokemonConfig(Nidoqueen,74,{{Blizzard,1.0f}, {Earthquake,1.0f}, {Thunderbolt,1.0f}, {BodySlam,0.6554f}, {Substitute,0.3446f}}),
            PokemonConfig(Nidoran_F,90,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Thunderbolt,1.0f}, {DoubleEdge,0.5129f}, {DoubleKick,0.4871f}}),
            PokemonConfig(Nidoran_M,90,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Thunderbolt,1.0f}, {DoubleKick,0.5077f}, {DoubleEdge,0.4923f}}),
            PokemonConfig(Nidorina,82,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Thunderbolt,1.0f}, {DoubleEdge,0.3398f}, {BubbleBeam,0.3311f}, {DoubleKick,0.3291f}}),
            PokemonConfig(Nidorino,82,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Thunderbolt,1.0f}, {DoubleEdge,0.3361f}, {DoubleKick,0.3341f}, {BubbleBeam,0.3298f}}),
            PokemonConfig(Ninetales,74,{{BodySlam,1.0f}, {ConfuseRay,1.0f}, {FireBlast,1.0f}, {Substitute,0.4074f}, {Flamethrower,0.1985f}, {Reflect,0.1978f}, {HyperBeam,0.1963f}}),
            PokemonConfig(Oddish,90,{{DoubleEdge,1.0f}, {MegaDrain,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.6592f}, {SwordsDance,0.3408f}}),
            PokemonConfig(Omanyte,87,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Rest,1.0f}, {Surf,0.5018f}, {HydroPump,0.4982f}}),
            PokemonConfig(Omastar,74,{{Blizzard,1.0f}, {BodySlam,0.6774f}, {SeismicToss,0.6761f}, {Rest,0.6466f}, {HydroPump,0.5103f}, {Surf,0.4897f}}),
            PokemonConfig(Onix,80,{{BodySlam,1.0f}, {Earthquake,1.0f}, {Explosion,1.0f}, {RockSlide,1.0f}}),
            PokemonConfig(Paras,90,{{BodySlam,1.0f}, {MegaDrain,1.0f}, {Spore,1.0f}, {StunSpore,0.6764f}, {SwordsDance,0.3236f}}),
            PokemonConfig(Parasect,77,{{BodySlam,1.0f}, {MegaDrain,1.0f}, {Spore,1.0f}, {StunSpore,0.4311f}, {SwordsDance,0.2857f}, {Slash,0.1442f}, {HyperBeam,0.139f}}),
            PokemonConfig(Persian,73,{{BodySlam,1.0f}, {BubbleBeam,1.0f}, {Slash,1.0f}, {Thunderbolt,0.5034f}, {HyperBeam,0.4966f}}),
            PokemonConfig(Pidgeot,76,{{Agility,1.0f}, {DoubleEdge,1.0f}, {HyperBeam,1.0f}, {QuickAttack,0.3291f}, {SkyAttack,0.228f}, {Reflect,0.1199f}, {SandAttack,0.1092f}, {Substitute,0.1071f}, {MirrorMove,0.1067f}}),
            PokemonConfig(Pidgeotto,85,{{DoubleEdge,1.0f}, {QuickAttack,0.8872f}, {Agility,0.8803f}, {SkyAttack,0.7241f}, {SandAttack,0.1728f}, {MirrorMove,0.1704f}, {Substitute,0.1652f}}),
            PokemonConfig(Pidgey,93,{{DoubleEdge,1.0f}, {Agility,0.8889f}, {QuickAttack,0.8771f}, {SkyAttack,0.7367f}, {SandAttack,0.1736f}, {Substitute,0.1654f}, {MirrorMove,0.1584f}}),
            PokemonConfig(Pikachu,87,{{Surf,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {SeismicToss,0.3976f}, {Thunder,0.2058f}, {BodySlam,0.2039f}, {Agility,0.1927f}}),
            PokemonConfig(Pinsir,75,{{HyperBeam,1.0f}, {SwordsDance,1.0f}, {Submission,0.6715f}, {BodySlam,0.6691f}, {Slash,0.3309f}, {SeismicToss,0.3285f}}),
            PokemonConfig(Poliwag,86,{{Amnesia,1.0f}, {Blizzard,1.0f}, {Hypnosis,1.0f}, {Surf,1.0f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Poliwhirl,79,{{Amnesia,1.0f}, {Blizzard,1.0f}, {Hypnosis,1.0f}, {Surf,1.0f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Poliwrath,74,{{Blizzard,1.0f}, {Surf,1.0f}, {Hypnosis,0.7501f}, {Amnesia,0.494f}, {BodySlam,0.2595f}, {Submission,0.2552f}, {Earthquake,0.2412f}}),
            PokemonConfig(Ponyta,84,{{Agility,1.0f}, {BodySlam,1.0f}, {FireBlast,1.0f}, {Reflect,0.5081f}, {Substitute,0.3324f}, {Stomp,0.1594f}}),
            PokemonConfig(Porygon,76,{{Blizzard,1.0f}, {Recover,1.0f}, {ThunderWave,1.0f}, {DoubleEdge,0.2567f}, {Thunderbolt,0.2536f}, {TriAttack,0.2483f}, {Psychic,0.2413f}}),
            PokemonConfig(Primeape,76,{{BodySlam,1.0f}, {Submission,1.0f}, {RockSlide,0.744f}, {HyperBeam,0.5124f}, {Thunderbolt,0.256f}, {Counter,0.2519f}, {LowKick,0.2357f}}),
            PokemonConfig(Psyduck,89,{{Amnesia,1.0f}, {Blizzard,1.0f}, {Surf,1.0f}, {SeismicToss,0.4053f}, {BodySlam,0.2045f}, {Rest,0.1964f}, {HydroPump,0.1939f}}),
            PokemonConfig(Raichu,74,{{Surf,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {HyperBeam,0.3436f}, {Agility,0.1676f}, {BodySlam,0.1662f}, {Thunder,0.166f}, {SeismicToss,0.1566f}}),
            PokemonConfig(Rapidash,75,{{Agility,1.0f}, {BodySlam,1.0f}, {FireBlast,1.0f}, {HyperBeam,1.0f}}),
            PokemonConfig(Raticate,75,{{Blizzard,1.0f}, {BodySlam,1.0f}, {HyperBeam,1.0f}, {SuperFang,1.0f}}),
            PokemonConfig(Rattata,89,{{Blizzard,1.0f}, {BodySlam,1.0f}, {SuperFang,1.0f}, {Thunderbolt,0.4999f}, {QuickAttack,0.3324f}, {DoubleEdge,0.1677f}}),
            PokemonConfig(Rhydon,68,{{BodySlam,1.0f}, {Earthquake,1.0f}, {RockSlide,1.0f}, {Substitute,1.0f}}),
            PokemonConfig(Rhyhorn,84,{{BodySlam,1.0f}, {Earthquake,1.0f}, {RockSlide,1.0f}, {Substitute,1.0f}}),
            PokemonConfig(Sandshrew,88,{{BodySlam,1.0f}, {Earthquake,1.0f}, {RockSlide,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Sandslash,76,{{BodySlam,1.0f}, {Earthquake,1.0f}, {RockSlide,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Scyther,75,{{Agility,1.0f}, {HyperBeam,1.0f}, {Slash,1.0f}, {SwordsDance,1.0f}}),
            PokemonConfig(Seadra,77,{{Agility,1.0f}, {Blizzard,1.0f}, {Surf,1.0f}, {DoubleEdge,0.2543f}, {HydroPump,0.2504f}, {Smokescreen,0.2484f}, {HyperBeam,0.2469f}}),
            PokemonConfig(Seaking,78,{{Blizzard,1.0f}, {Surf,1.0f}, {DoubleEdge,0.672f}, {HyperBeam,0.6711f}, {Agility,0.6569f}}),
            PokemonConfig(Seel,88,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Rest,1.0f}, {Surf,1.0f}}),
            PokemonConfig(Shellder,90,{{Blizzard,1.0f}, {DoubleEdge,1.0f}, {Explosion,1.0f}, {Surf,1.0f}}),
            PokemonConfig(Slowbro,68,{{Amnesia,1.0f}, {ThunderWave,1.0f}, {Surf,0.8301f}, {Rest,0.4955f}, {Psychic,0.3409f}, {Blizzard,0.3335f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Slowpoke,84,{{Amnesia,1.0f}, {ThunderWave,1.0f}, {Surf,0.83f}, {Rest,0.5035f}, {Psychic,0.3381f}, {Blizzard,0.3284f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Snorlax,69,{{BodySlam,1.0f}, {SelfDestruct,0.7433f}, {Amnesia,0.5011f}, {Blizzard,0.5011f}, {Earthquake,0.4989f}, {HyperBeam,0.4989f}, {Rest,0.2567f}}),
            PokemonConfig(Spearow,89,{{Agility,1.0f}, {DoubleEdge,1.0f}, {DrillPeck,1.0f}, {Substitute,0.255f}, {MirrorMove,0.2504f}, {Leer,0.2475f}, {Mimic,0.2472f}}),
            PokemonConfig(Squirtle,90,{{Blizzard,1.0f}, {SeismicToss,1.0f}, {Surf,0.6852f}, {BodySlam,0.5047f}, {Counter,0.4953f}, {HydroPump,0.3148f}}),
            PokemonConfig(Starmie,68,{{Recover,1.0f}, {ThunderWave,0.7359f}, {Psychic,0.5665f}, {Surf,0.4971f}, {Blizzard,0.4758f}, {Thunderbolt,0.4702f}, {HydroPump,0.2545f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Staryu,84,{{Recover,1.0f}, {Blizzard,0.6803f}, {Thunderbolt,0.6734f}, {Surf,0.6703f}, {ThunderWave,0.6463f}, {HydroPump,0.3297f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Tangela,74,{{BodySlam,1.0f}, {MegaDrain,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.5031f}, {SwordsDance,0.3232f}, {Growth,0.1737f}}),
            PokemonConfig(Tauros,68,{{BodySlam,1.0f}, {Earthquake,1.0f}, {HyperBeam,1.0f}, {Blizzard,0.7429f}, {Thunderbolt,0.2571f}}),
            PokemonConfig(Tentacool,86,{{Blizzard,1.0f}, {MegaDrain,1.0f}, {Surf,1.0f}, {HydroPump,0.6764f}, {Barrier,0.3236f}}, zero_atk_evs, two_atk_ivs),
            PokemonConfig(Tentacruel,73,{{Blizzard,1.0f}, {HyperBeam,1.0f}, {SwordsDance,1.0f}, {Surf,0.6708f}, {HydroPump,0.3292f}}),
            PokemonConfig(Vaporeon,74,{{Blizzard,1.0f}, {Rest,1.0f}, {Surf,1.0f}, {BodySlam,0.4922f}, {HydroPump,0.1729f}, {Mimic,0.1704f}, {AcidArmor,0.1645f}}),
            PokemonConfig(Venomoth,74,{{Psychic,1.0f}, {SleepPowder,1.0f}, {StunSpore,1.0f}, {MegaDrain,0.5101f}, {DoubleEdge,0.4899f}}),
            PokemonConfig(Venonat,88,{{Psychic,1.0f}, {SleepPowder,1.0f}, {StunSpore,1.0f}, {MegaDrain,0.5009f}, {DoubleEdge,0.4991f}}),
            PokemonConfig(Venusaur,74,{{BodySlam,1.0f}, {RazorLeaf,1.0f}, {SleepPowder,1.0f}, {SwordsDance,0.6635f}, {HyperBeam,0.3365f}}),
            PokemonConfig(Victreebel,74,{{BodySlam,1.0f}, {RazorLeaf,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.4976f}, {SwordsDance,0.3245f}, {HyperBeam,0.1779f}}),
            PokemonConfig(Vileplume,76,{{BodySlam,1.0f}, {MegaDrain,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.6718f}, {SwordsDance,0.3282f}}),
            PokemonConfig(Voltorb,88,{{Explosion,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}, {TakeDown,0.5013f}, {Thunder,0.4987f}}),
            PokemonConfig(Vulpix,88,{{BodySlam,1.0f}, {ConfuseRay,1.0f}, {FireBlast,1.0f}, {Flamethrower,0.3441f}, {Substitute,0.3383f}, {QuickAttack,0.1623f}, {Reflect,0.1553f}}),
            PokemonConfig(Wartortle,82,{{Blizzard,1.0f}, {BodySlam,1.0f}, {Surf,0.6786f}, {Counter,0.3347f}, {Rest,0.3347f}, {SeismicToss,0.3306f}, {HydroPump,0.3214f}}),
            PokemonConfig(Weepinbell,80,{{DoubleEdge,1.0f}, {RazorLeaf,1.0f}, {SleepPowder,1.0f}, {StunSpore,0.6593f}, {SwordsDance,0.3407f}}),
            PokemonConfig(Weezing,76,{{Explosion,1.0f}, {FireBlast,1.0f}, {Sludge,1.0f}, {Thunderbolt,1.0f}}),
            PokemonConfig(Wigglytuff,76,{{Blizzard,1.0f}, {BodySlam,1.0f}, {ThunderWave,1.0f}, {Sing,0.3361f}, {HyperBeam,0.3322f}, {Counter,0.3317f}}),
            PokemonConfig(Zapdos,68,{{Agility,1.0f}, {DrillPeck,1.0f}, {ThunderWave,1.0f}, {Thunderbolt,1.0f}}),
            PokemonConfig(Zubat, 100, { {ConfuseRay,1.0f}, {DoubleEdge,1.0f}, {MegaDrain,1.0f}, {Substitute,0.6461f}, {WingAttack,0.3539f} })
        };
    }
    const std::vector<PokemonConfig> gen1RandBatsSets = initializeConfigs();

    std::array<Move, 4> sampleMoves(const std::vector<std::pair<Move, float>>& moves, std::mt19937& gen) {
        std::array<Move, 4> chosenMoves = { NullMove };
        std::vector<std::pair<Move, float>> weightedMoves;
        uint8_t moves_chosen = 0;

        for (const auto& move : moves) {
            if (move.second == 1.0) {
                if (moves_chosen < 4) {
                    chosenMoves[moves_chosen] = move.first;
                    moves_chosen++;
                }
            }
            else {
                weightedMoves.push_back(move);
            }
        }

        while (moves_chosen < 4 && !weightedMoves.empty()) {
            std::vector<float> weights;
            for (const auto& move : weightedMoves) {
                weights.push_back(move.second);
            }
            if (weights.empty()) break;

            std::discrete_distribution<> move_dist(weights.begin(), weights.end());
            int idx = move_dist(gen);

            if (moves_chosen < 4) {
                chosenMoves[moves_chosen] = weightedMoves[idx].first;
                moves_chosen++;
                weightedMoves.erase(weightedMoves.begin() + idx); // Remove chosen move
            }
        }

        return chosenMoves;
    }

    Gen1Side generateRandomTeam() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> pokeDist(0, static_cast<int>(gen1RandBatsSets.size()) - 1);
        std::array<Gen1Poke, 6> pokemons;
        for (int i = 0; i < 6; ++i) {
            const PokemonConfig& pokemon = gen1RandBatsSets[pokeDist(gen)];
            pokemons[i] = Gen1Poke(pokemon.species, sampleMoves(pokemon.moves, gen), pokemon.level, pokemon.EV, pokemon.IV);
        }
        return make_side(pokemons);
    }

    Gen1Battle generateRandomBattle(uint64_t seed) {
        return Gen1Battle(generateRandomTeam(), generateRandomTeam(), seed);
    }

    Gen1Battle generateRandomBattle(Gen1Agent* a1, Gen1Agent* a2, uint64_t seed) {
        return Gen1Battle(a1, a2, generateRandomTeam(), generateRandomTeam(), seed);
    }

}

