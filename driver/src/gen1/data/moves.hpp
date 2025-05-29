#pragma once

#include <cstdint>
#include <assert.h>
#include "types.hpp"

namespace pkmndriver {
	namespace Gen1 {
        enum Move : std::uint8_t {
            NullMove,
            Pound,
            KarateChop,
            DoubleSlap,
            CometPunch,
            MegaPunch,
            PayDay,
            FirePunch,
            IcePunch,
            ThunderPunch,
            Scratch,
            ViseGrip,
            Guillotine,
            RazorWind,
            SwordsDance,
            Cut,
            Gust,
            WingAttack,
            Whirlwind,
            Fly,
            Bind,
            Slam,
            VineWhip,
            Stomp,
            DoubleKick,
            MegaKick,
            JumpKick,
            RollingKick,
            SandAttack,
            Headbutt,
            HornAttack,
            FuryAttack,
            HornDrill,
            Tackle,
            BodySlam,
            Wrap,
            TakeDown,
            Thrash,
            DoubleEdge,
            TailWhip,
            PoisonSting,
            Twineedle,
            PinMissile,
            Leer,
            Bite,
            Growl,
            Roar,
            Sing,
            Supersonic,
            SonicBoom,
            Disable,
            Acid,
            Ember,
            Flamethrower,
            Mist,
            WaterGun,
            HydroPump,
            Surf,
            IceBeam,
            Blizzard,
            Psybeam,
            BubbleBeam,
            AuroraBeam,
            HyperBeam,
            Peck,
            DrillPeck,
            Submission,
            LowKick,
            Counter,
            SeismicToss,
            Strength,
            Absorb,
            MegaDrain,
            LeechSeed,
            Growth,
            RazorLeaf,
            SolarBeam,
            PoisonPowder,
            StunSpore,
            SleepPowder,
            PetalDance,
            StringShot,
            DragonRage,
            FireSpin,
            ThunderShock,
            Thunderbolt,
            ThunderWave,
            Thunder,
            RockThrow,
            Earthquake,
            Fissure,
            Dig,
            Toxic,
            Confusion,
            Psychic,
            Hypnosis,
            Meditate,
            Agility,
            QuickAttack,
            Rage,
            Teleport,
            NightShade,
            Mimic,
            Screech,
            DoubleTeam,
            Recover,
            Harden,
            Minimize,
            Smokescreen,
            ConfuseRay,
            Withdraw,
            DefenseCurl,
            Barrier,
            LightScreen,
            Haze,
            Reflect,
            FocusEnergy,
            Bide,
            Metronome,
            MirrorMove,
            SelfDestruct,
            EggBomb,
            Lick,
            Smog,
            Sludge,
            BoneClub,
            FireBlast,
            Waterfall,
            Clamp,
            Swift,
            SkullBash,
            SpikeCannon,
            Constrict,
            Amnesia,
            Kinesis,
            SoftBoiled,
            HighJumpKick,
            Glare,
            DreamEater,
            PoisonGas,
            Barrage,
            LeechLife,
            LovelyKiss,
            SkyAttack,
            Transform,
            Bubble,
            DizzyPunch,
            Spore,
            Flash,
            Psywave,
            Splash,
            AcidArmor,
            Crabhammer,
            Explosion,
            FurySwipes,
            Bonemerang,
            Rest,
            RockSlide,
            HyperFang,
            Sharpen,
            Conversion,
            TriAttack,
            SuperFang,
            Slash,
            Substitute,
            Struggle,
        };
        constexpr uint8_t move_pp(Move move, float PP_UP = 3) {
            PP_UP = 1.0f + 0.2f*PP_UP;
            switch (move) {
            case NullMove:      return static_cast<uint8_t>(0*PP_UP);
            case Pound:         return static_cast<uint8_t>(35*PP_UP);
            case KarateChop:    return static_cast<uint8_t>(25*PP_UP);
            case DoubleSlap:    return static_cast<uint8_t>(10*PP_UP);
            case CometPunch:    return static_cast<uint8_t>(15*PP_UP);
            case MegaPunch:     return static_cast<uint8_t>(20*PP_UP);
            case PayDay:        return static_cast<uint8_t>(20*PP_UP);
            case FirePunch:     return static_cast<uint8_t>(15*PP_UP);
            case IcePunch:      return static_cast<uint8_t>(15*PP_UP);
            case ThunderPunch:  return static_cast<uint8_t>(15*PP_UP);
            case Scratch:       return static_cast<uint8_t>(35*PP_UP);
            case ViseGrip:      return static_cast<uint8_t>(30*PP_UP);
            case Guillotine:    return static_cast<uint8_t>(5*PP_UP);
            case RazorWind:     return static_cast<uint8_t>(10*PP_UP);
            case SwordsDance:   return static_cast<uint8_t>(30*PP_UP);
            case Cut:           return static_cast<uint8_t>(30*PP_UP);
            case Gust:          return static_cast<uint8_t>(35*PP_UP);
            case WingAttack:    return static_cast<uint8_t>(35*PP_UP);
            case Whirlwind:     return static_cast<uint8_t>(20*PP_UP);
            case Fly:           return static_cast<uint8_t>(15*PP_UP);
            case Bind:          return static_cast<uint8_t>(20*PP_UP);
            case Slam:          return static_cast<uint8_t>(20*PP_UP);
            case VineWhip:      return static_cast<uint8_t>(10*PP_UP);
            case Stomp:         return static_cast<uint8_t>(20*PP_UP);
            case DoubleKick:    return static_cast<uint8_t>(30*PP_UP);
            case MegaKick:      return static_cast<uint8_t>(5*PP_UP);
            case JumpKick:      return static_cast<uint8_t>(25*PP_UP);
            case RollingKick:   return static_cast<uint8_t>(15*PP_UP);
            case SandAttack:    return static_cast<uint8_t>(15*PP_UP);
            case Headbutt:      return static_cast<uint8_t>(15*PP_UP);
            case HornAttack:    return static_cast<uint8_t>(25*PP_UP);
            case FuryAttack:    return static_cast<uint8_t>(20*PP_UP);
            case HornDrill:     return static_cast<uint8_t>(5*PP_UP);
            case Tackle:        return static_cast<uint8_t>(35*PP_UP);
            case BodySlam:      return static_cast<uint8_t>(15*PP_UP);
            case Wrap:          return static_cast<uint8_t>(20*PP_UP);
            case TakeDown:      return static_cast<uint8_t>(20*PP_UP);
            case Thrash:        return static_cast<uint8_t>(20*PP_UP);
            case DoubleEdge:    return static_cast<uint8_t>(15*PP_UP);
            case TailWhip:      return static_cast<uint8_t>(30*PP_UP);
            case PoisonSting:   return static_cast<uint8_t>(35*PP_UP);
            case Twineedle:     return static_cast<uint8_t>(20*PP_UP);
            case PinMissile:    return static_cast<uint8_t>(20*PP_UP);
            case Leer:          return static_cast<uint8_t>(30*PP_UP);
            case Bite:          return static_cast<uint8_t>(25*PP_UP);
            case Growl:         return static_cast<uint8_t>(40*PP_UP);
            case Roar:          return static_cast<uint8_t>(20*PP_UP);
            case Sing:          return static_cast<uint8_t>(15*PP_UP);
            case Supersonic:    return static_cast<uint8_t>(20*PP_UP);
            case SonicBoom:     return static_cast<uint8_t>(20*PP_UP);
            case Disable:       return static_cast<uint8_t>(20*PP_UP);
            case Acid:          return static_cast<uint8_t>(30*PP_UP);
            case Ember:         return static_cast<uint8_t>(25*PP_UP);
            case Flamethrower:  return static_cast<uint8_t>(15*PP_UP);
            case Mist:          return static_cast<uint8_t>(30*PP_UP);
            case WaterGun:      return static_cast<uint8_t>(25*PP_UP);
            case HydroPump:     return static_cast<uint8_t>(5*PP_UP);
            case Surf:          return static_cast<uint8_t>(15*PP_UP);
            case IceBeam:       return static_cast<uint8_t>(10*PP_UP);
            case Blizzard:      return static_cast<uint8_t>(5*PP_UP);
            case Psybeam:       return static_cast<uint8_t>(20*PP_UP);
            case BubbleBeam:    return static_cast<uint8_t>(20*PP_UP);
            case AuroraBeam:    return static_cast<uint8_t>(20*PP_UP);
            case HyperBeam:     return static_cast<uint8_t>(5*PP_UP);
            case Peck:          return static_cast<uint8_t>(35*PP_UP);
            case DrillPeck:     return static_cast<uint8_t>(20*PP_UP);
            case Submission:    return static_cast<uint8_t>(25*PP_UP);
            case LowKick:       return static_cast<uint8_t>(20*PP_UP);
            case Counter:       return static_cast<uint8_t>(20*PP_UP);
            case SeismicToss:   return static_cast<uint8_t>(20*PP_UP);
            case Strength:      return static_cast<uint8_t>(15*PP_UP);
            case Absorb:        return static_cast<uint8_t>(20*PP_UP);
            case MegaDrain:     return static_cast<uint8_t>(10*PP_UP);
            case LeechSeed:     return static_cast<uint8_t>(10*PP_UP);
            case Growth:        return static_cast<uint8_t>(40*PP_UP);
            case RazorLeaf:     return static_cast<uint8_t>(25*PP_UP);
            case SolarBeam:     return static_cast<uint8_t>(10*PP_UP);
            case PoisonPowder:  return static_cast<uint8_t>(35*PP_UP);
            case StunSpore:     return static_cast<uint8_t>(30*PP_UP);
            case SleepPowder:   return static_cast<uint8_t>(15*PP_UP);
            case PetalDance:    return static_cast<uint8_t>(20*PP_UP);
            case StringShot:    return static_cast<uint8_t>(40*PP_UP);
            case DragonRage:    return static_cast<uint8_t>(10*PP_UP);
            case FireSpin:      return static_cast<uint8_t>(15*PP_UP);
            case ThunderShock:  return static_cast<uint8_t>(30*PP_UP);
            case Thunderbolt:   return static_cast<uint8_t>(15*PP_UP);
            case ThunderWave:   return static_cast<uint8_t>(20*PP_UP);
            case Thunder:       return static_cast<uint8_t>(10*PP_UP);
            case RockThrow:     return static_cast<uint8_t>(15*PP_UP);
            case Earthquake:    return static_cast<uint8_t>(10*PP_UP);
            case Fissure:       return static_cast<uint8_t>(5*PP_UP);
            case Dig:           return static_cast<uint8_t>(10*PP_UP);
            case Toxic:         return static_cast<uint8_t>(10*PP_UP);
            case Confusion:     return static_cast<uint8_t>(25*PP_UP);
            case Psychic:       return static_cast<uint8_t>(10*PP_UP);
            case Hypnosis:      return static_cast<uint8_t>(20*PP_UP);
            case Meditate:      return static_cast<uint8_t>(40*PP_UP);
            case Agility:       return static_cast<uint8_t>(30*PP_UP);
            case QuickAttack:   return static_cast<uint8_t>(30*PP_UP);
            case Rage:          return static_cast<uint8_t>(20*PP_UP);
            case Teleport:      return static_cast<uint8_t>(20*PP_UP);
            case NightShade:    return static_cast<uint8_t>(15*PP_UP);
            case Mimic:         return static_cast<uint8_t>(10*PP_UP);
            case Screech:       return static_cast<uint8_t>(40*PP_UP);
            case DoubleTeam:    return static_cast<uint8_t>(15*PP_UP);
            case Recover:       return static_cast<uint8_t>(20*PP_UP);
            case Harden:        return static_cast<uint8_t>(30*PP_UP);
            case Minimize:      return static_cast<uint8_t>(20*PP_UP);
            case Smokescreen:   return static_cast<uint8_t>(20*PP_UP);
            case ConfuseRay:    return static_cast<uint8_t>(10*PP_UP);
            case Withdraw:      return static_cast<uint8_t>(40*PP_UP);
            case DefenseCurl:   return static_cast<uint8_t>(40*PP_UP);
            case Barrier:       return static_cast<uint8_t>(30*PP_UP);
            case LightScreen:   return static_cast<uint8_t>(30*PP_UP);
            case Haze:          return static_cast<uint8_t>(30*PP_UP);
            case Reflect:       return static_cast<uint8_t>(20*PP_UP);
            case FocusEnergy:   return static_cast<uint8_t>(30*PP_UP);
            case Bide:          return static_cast<uint8_t>(10*PP_UP);
            case Metronome:     return static_cast<uint8_t>(10*PP_UP);
            case MirrorMove:    return static_cast<uint8_t>(20*PP_UP);
            case SelfDestruct:  return static_cast<uint8_t>(5*PP_UP);
            case EggBomb:       return static_cast<uint8_t>(10*PP_UP);
            case Lick:          return static_cast<uint8_t>(30*PP_UP);
            case Smog:          return static_cast<uint8_t>(20*PP_UP);
            case Sludge:        return static_cast<uint8_t>(20*PP_UP);
            case BoneClub:      return static_cast<uint8_t>(20*PP_UP);
            case FireBlast:     return static_cast<uint8_t>(5*PP_UP);
            case Waterfall:     return static_cast<uint8_t>(15*PP_UP);
            case Clamp:         return static_cast<uint8_t>(10*PP_UP);
            case Swift:         return static_cast<uint8_t>(20*PP_UP);
            case SkullBash:     return static_cast<uint8_t>(15*PP_UP);
            case SpikeCannon:   return static_cast<uint8_t>(15*PP_UP);
            case Constrict:     return static_cast<uint8_t>(35*PP_UP);
            case Amnesia:       return static_cast<uint8_t>(20*PP_UP);
            case Kinesis:       return static_cast<uint8_t>(15*PP_UP);
            case SoftBoiled:    return static_cast<uint8_t>(10*PP_UP);
            case HighJumpKick:  return static_cast<uint8_t>(20*PP_UP);
            case Glare:         return static_cast<uint8_t>(30*PP_UP);
            case DreamEater:    return static_cast<uint8_t>(15*PP_UP);
            case PoisonGas:     return static_cast<uint8_t>(40*PP_UP);
            case Barrage:       return static_cast<uint8_t>(20*PP_UP);
            case LeechLife:     return static_cast<uint8_t>(15*PP_UP);
            case LovelyKiss:    return static_cast<uint8_t>(10*PP_UP);
            case SkyAttack:     return static_cast<uint8_t>(5*PP_UP);
            case Transform:     return static_cast<uint8_t>(10*PP_UP);
            case Bubble:        return static_cast<uint8_t>(30*PP_UP);
            case DizzyPunch:    return static_cast<uint8_t>(10*PP_UP);
            case Spore:         return static_cast<uint8_t>(15*PP_UP);
            case Flash:         return static_cast<uint8_t>(20*PP_UP);
            case Psywave:       return static_cast<uint8_t>(15*PP_UP);
            case Splash:        return static_cast<uint8_t>(40*PP_UP);
            case AcidArmor:     return static_cast<uint8_t>(40*PP_UP);
            case Crabhammer:    return static_cast<uint8_t>(10*PP_UP);
            case Explosion:     return static_cast<uint8_t>(5*PP_UP);
            case FurySwipes:    return static_cast<uint8_t>(15*PP_UP);
            case Bonemerang:    return static_cast<uint8_t>(10*PP_UP);
            case Rest:          return static_cast<uint8_t>(10*PP_UP);
            case RockSlide:     return static_cast<uint8_t>(10*PP_UP);
            case HyperFang:     return static_cast<uint8_t>(15*PP_UP);
            case Sharpen:       return static_cast<uint8_t>(30*PP_UP);
            case Conversion:    return static_cast<uint8_t>(30*PP_UP);
            case TriAttack:     return static_cast<uint8_t>(10*PP_UP);
            case SuperFang:     return static_cast<uint8_t>(10*PP_UP);
            case Slash:         return static_cast<uint8_t>(20*PP_UP);
            case Substitute:    return static_cast<uint8_t>(10*PP_UP);
            case Struggle:      return static_cast<uint8_t>(10*PP_UP);
            default: return 0;
            }
        }
        namespace MoveData {
            enum Effect : uint8_t {
                None,
                // onBegin
                Confusion,
                Conversion,
                FocusEnergy,
                Haze,
                Heal,
                LeechSeed,
                LightScreen,
                Mimic,
                Mist,
                Paralyze,
                Poison,
                Reflect,
                Splash,
                Substitute,
                SwitchAndTeleport,
                Transform,
                // onEnd
                AccuracyDown1,
                AttackDown1,
                DefenseDown1,
                DefenseDown2,
                SpeedDown1,
                AttackUp1,
                AttackUp2,
                Bide,
                DefenseUp1,
                DefenseUp2,
                EvasionUp1,
                Sleep,
                SpecialUp1,
                SpecialUp2,
                SpeedUp2,
                // isSpecial
                DrainHP,
                DreamEater,
                Explode,
                JumpKick,
                PayDay,
                Rage,
                Recoil,
                Binding,
                Charge,
                SpecialDamage,
                SuperFang,
                Swift,
                Thrashing,
                // isMulti
                DoubleHit,
                MultiHit,
                Twineedle,
                // other
                AttackDownChance,
                DefenseDownChance,
                SpeedDownChance,
                SpecialDownChance,
                BurnChance1,
                BurnChance2,
                ConfusionChance,
                FlinchChance1,
                FlinchChance2,
                FreezeChance,
                ParalyzeChance1,
                ParalyzeChance2,
                PoisonChance1,
                PoisonChance2,
                Disable,
                HighCritical,
                HyperBeam,
                Metronome,
                MirrorMove,
                OHKO,
            };
            enum Target : uint8_t {
                // none
                All,
                AllySide,
                Field,
                Self,
                // resolve
                AllOthers,
                Depends,
                Other,
                Any,
                Allies,
                Ally,
                AllyOrSelf,
                Foe,
                // resolve + run
                Foes,
                FoeSide,
                RandomFoe,
            };
            struct MoveData {
                Effect effect;
                uint8_t bp;
                Type type;
                uint8_t accuracy;
                Target target;
            };
            inline std::array<MoveData, 165> DATA = { {
                {.effect = None, .bp = 40, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = HighCritical, .bp = 50, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = MultiHit, .bp = 15, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = MultiHit, .bp = 18, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = None, .bp = 80, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = PayDay, .bp = 40, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = BurnChance1, .bp = 75, .type = Fire, .accuracy = 100, .target = Other},
                {.effect = FreezeChance, .bp = 75, .type = Ice, .accuracy = 100, .target = Other},
                {.effect = ParalyzeChance1, .bp = 75, .type = Electric, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 40, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 55, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = OHKO, .bp = 0, .type = Normal, .accuracy = 30, .target = Other},
                {.effect = Charge, .bp = 80, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = AttackUp2, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = None, .bp = 50, .type = Normal, .accuracy = 95, .target = Other},
                {.effect = None, .bp = 40, .type = Normal, .accuracy = 100, .target = Any},
                {.effect = None, .bp = 35, .type = Flying, .accuracy = 100, .target = Any},
                {.effect = SwitchAndTeleport, .bp = 0, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = Charge, .bp = 70, .type = Flying, .accuracy = 95, .target = Any},
                {.effect = Binding, .bp = 15, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = None, .bp = 80, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = None, .bp = 35, .type = Grass, .accuracy = 100, .target = Other},
                {.effect = FlinchChance2, .bp = 65, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = DoubleHit, .bp = 30, .type = Fighting, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 120, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = JumpKick, .bp = 70, .type = Fighting, .accuracy = 95, .target = Other},
                {.effect = FlinchChance2, .bp = 60, .type = Fighting, .accuracy = 85, .target = Other},
                {.effect = AccuracyDown1, .bp = 0, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = FlinchChance2, .bp = 70, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 65, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = MultiHit, .bp = 15, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = OHKO, .bp = 0, .type = Normal, .accuracy = 30, .target = Other},
                {.effect = None, .bp = 35, .type = Normal, .accuracy = 95, .target = Other},
                {.effect = ParalyzeChance2, .bp = 85, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = Binding, .bp = 15, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = Recoil, .bp = 90, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = Thrashing, .bp = 90, .type = Normal, .accuracy = 100, .target = RandomFoe},
                {.effect = Recoil, .bp = 100, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = DefenseDown1, .bp = 0, .type = Normal, .accuracy = 100, .target = Foes},
                {.effect = PoisonChance1, .bp = 15, .type = Type::Poison, .accuracy = 100, .target = Other},
                {.effect = Twineedle, .bp = 25, .type = Bug, .accuracy = 100, .target = Other},
                {.effect = MultiHit, .bp = 14, .type = Bug, .accuracy = 85, .target = Other},
                {.effect = DefenseDown1, .bp = 0, .type = Normal, .accuracy = 100, .target = Foes},
                {.effect = FlinchChance1, .bp = 60, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = AttackDown1, .bp = 0, .type = Normal, .accuracy = 100, .target = Foes},
                {.effect = SwitchAndTeleport, .bp = 0, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = Sleep, .bp = 0, .type = Normal, .accuracy = 55, .target = Other},
                {.effect = Confusion, .bp = 0, .type = Normal, .accuracy = 55, .target = Other},
                {.effect = SpecialDamage, .bp = 1, .type = Normal, .accuracy = 90, .target = Other},
                {.effect = Disable, .bp = 0, .type = Normal, .accuracy = 55, .target = Other},
                {.effect = DefenseDownChance, .bp = 40, .type = Type::Poison, .accuracy = 100, .target = Other},
                {.effect = BurnChance1, .bp = 40, .type = Fire, .accuracy = 100, .target = Other},
                {.effect = BurnChance1, .bp = 95, .type = Fire, .accuracy = 100, .target = Other},
                {.effect = Mist, .bp = 0, .type = Ice, .accuracy = 100, .target = Self},
                {.effect = None, .bp = 40, .type = Water, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 120, .type = Water, .accuracy = 80, .target = Other},
                {.effect = None, .bp = 95, .type = Water, .accuracy = 100, .target = Foes},
                {.effect = FreezeChance, .bp = 95, .type = Ice, .accuracy = 100, .target = Other},
                {.effect = FreezeChance, .bp = 120, .type = Ice, .accuracy = 90, .target = Other},
                {.effect = ConfusionChance, .bp = 65, .type = Psychic_, .accuracy = 100, .target = Other},
                {.effect = SpeedDownChance, .bp = 65, .type = Water, .accuracy = 100, .target = Other},
                {.effect = AttackDownChance, .bp = 65, .type = Ice, .accuracy = 100, .target = Other},
                {.effect = HyperBeam, .bp = 150, .type = Normal, .accuracy = 90, .target = Other},
                {.effect = None, .bp = 35, .type = Flying, .accuracy = 100, .target = Any},
                {.effect = None, .bp = 80, .type = Flying, .accuracy = 100, .target = Any},
                {.effect = Recoil, .bp = 80, .type = Fighting, .accuracy = 80, .target = Other},
                {.effect = FlinchChance2, .bp = 50, .type = Fighting, .accuracy = 90, .target = Other},
                {.effect = None, .bp = 1, .type = Fighting, .accuracy = 100, .target = Depends},
                {.effect = SpecialDamage, .bp = 1, .type = Fighting, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 80, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = DrainHP, .bp = 20, .type = Grass, .accuracy = 100, .target = Other},
                {.effect = DrainHP, .bp = 40, .type = Grass, .accuracy = 100, .target = Other},
                {.effect = LeechSeed, .bp = 0, .type = Grass, .accuracy = 90, .target = Other},
                {.effect = SpecialUp1, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = HighCritical, .bp = 55, .type = Grass, .accuracy = 95, .target = Other},
                {.effect = Charge, .bp = 120, .type = Grass, .accuracy = 100, .target = Other},
                {.effect = Poison, .bp = 0, .type = Type::Poison, .accuracy = 75, .target = Other},
                {.effect = Paralyze, .bp = 0, .type = Grass, .accuracy = 75, .target = Other},
                {.effect = Sleep, .bp = 0, .type = Grass, .accuracy = 75, .target = Other},
                {.effect = Thrashing, .bp = 70, .type = Grass, .accuracy = 100, .target = RandomFoe},
                {.effect = SpeedDown1, .bp = 0, .type = Bug, .accuracy = 95, .target = Foes},
                {.effect = SpecialDamage, .bp = 1, .type = Dragon, .accuracy = 100, .target = Other},
                {.effect = Binding, .bp = 15, .type = Fire, .accuracy = 70, .target = Other},
                {.effect = ParalyzeChance1, .bp = 40, .type = Electric, .accuracy = 100, .target = Other},
                {.effect = ParalyzeChance1, .bp = 95, .type = Electric, .accuracy = 100, .target = Other},
                {.effect = Paralyze, .bp = 0, .type = Electric, .accuracy = 100, .target = Other},
                {.effect = ParalyzeChance1, .bp = 120, .type = Electric, .accuracy = 70, .target = Other},
                {.effect = None, .bp = 50, .type = Rock, .accuracy = 65, .target = Other},
                {.effect = None, .bp = 100, .type = Ground, .accuracy = 100, .target = AllOthers},
                {.effect = OHKO, .bp = 0, .type = Ground, .accuracy = 30, .target = Other},
                {.effect = Charge, .bp = 100, .type = Ground, .accuracy = 100, .target = Other},
                {.effect = Poison, .bp = 0, .type = Type::Poison, .accuracy = 85, .target = Other},
                {.effect = ConfusionChance, .bp = 50, .type = Psychic_, .accuracy = 100, .target = Other},
                {.effect = SpecialDownChance, .bp = 90, .type = Psychic_, .accuracy = 100, .target = Other},
                {.effect = Sleep, .bp = 0, .type = Psychic_, .accuracy = 60, .target = Other},
                {.effect = AttackUp1, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = SpeedUp2, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = None, .bp = 40, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = Rage, .bp = 20, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = SwitchAndTeleport, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = SpecialDamage, .bp = 1, .type = Ghost, .accuracy = 100, .target = Other},
                {.effect = Mimic, .bp = 0, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = DefenseDown2, .bp = 0, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = EvasionUp1, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = Heal, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = DefenseUp1, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = EvasionUp1, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = AccuracyDown1, .bp = 0, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = Confusion, .bp = 0, .type = Ghost, .accuracy = 100, .target = Other},
                {.effect = DefenseUp1, .bp = 0, .type = Water, .accuracy = 100, .target = Self},
                {.effect = DefenseUp1, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = DefenseUp2, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = LightScreen, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = Haze, .bp = 0, .type = Ice, .accuracy = 100, .target = Self},
                {.effect = Reflect, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = FocusEnergy, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = Bide, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = Metronome, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = MirrorMove, .bp = 0, .type = Flying, .accuracy = 100, .target = Self},
                {.effect = Explode, .bp = 130, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 100, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = ParalyzeChance2, .bp = 20, .type = Ghost, .accuracy = 100, .target = Other},
                {.effect = PoisonChance2, .bp = 20, .type = Type::Poison, .accuracy = 70, .target = Other},
                {.effect = PoisonChance2, .bp = 65, .type = Type::Poison, .accuracy = 100, .target = Other},
                {.effect = FlinchChance1, .bp = 65, .type = Ground, .accuracy = 85, .target = Other},
                {.effect = BurnChance2, .bp = 120, .type = Fire, .accuracy = 85, .target = Other},
                {.effect = None, .bp = 80, .type = Water, .accuracy = 100, .target = Other},
                {.effect = Binding, .bp = 35, .type = Water, .accuracy = 75, .target = Other},
                {.effect = Swift, .bp = 60, .type = Normal, .accuracy = 100, .target = Foes},
                {.effect = Charge, .bp = 100, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = MultiHit, .bp = 20, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = SpeedDownChance, .bp = 10, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = SpecialUp2, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = AccuracyDown1, .bp = 0, .type = Psychic_, .accuracy = 80, .target = Other},
                {.effect = Heal, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = JumpKick, .bp = 85, .type = Fighting, .accuracy = 90, .target = Other},
                {.effect = Paralyze, .bp = 0, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = DreamEater, .bp = 100, .type = Psychic_, .accuracy = 100, .target = Other},
                {.effect = Poison, .bp = 0, .type = Type::Poison, .accuracy = 55, .target = Other},
                {.effect = MultiHit, .bp = 15, .type = Normal, .accuracy = 85, .target = Other},
                {.effect = DrainHP, .bp = 20, .type = Bug, .accuracy = 100, .target = Other},
                {.effect = Sleep, .bp = 0, .type = Normal, .accuracy = 75, .target = Other},
                {.effect = Charge, .bp = 140, .type = Flying, .accuracy = 90, .target = Any},
                {.effect = Transform, .bp = 0, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = SpeedDownChance, .bp = 20, .type = Water, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 70, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = Sleep, .bp = 0, .type = Grass, .accuracy = 100, .target = Other},
                {.effect = AccuracyDown1, .bp = 0, .type = Normal, .accuracy = 70, .target = Other},
                {.effect = SpecialDamage, .bp = 1, .type = Psychic_, .accuracy = 80, .target = Other},
                {.effect = Splash, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = DefenseUp2, .bp = 0, .type = Type::Poison, .accuracy = 100, .target = Self},
                {.effect = HighCritical, .bp = 90, .type = Water, .accuracy = 85, .target = Other},
                {.effect = Explode, .bp = 170, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = MultiHit, .bp = 18, .type = Normal, .accuracy = 80, .target = Other},
                {.effect = DoubleHit, .bp = 50, .type = Ground, .accuracy = 90, .target = Other},
                {.effect = Heal, .bp = 0, .type = Psychic_, .accuracy = 100, .target = Self},
                {.effect = None, .bp = 75, .type = Rock, .accuracy = 90, .target = Other},
                {.effect = FlinchChance1, .bp = 80, .type = Normal, .accuracy = 90, .target = Other},
                {.effect = AttackUp1, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = Conversion, .bp = 0, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = None, .bp = 80, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = SuperFang, .bp = 1, .type = Normal, .accuracy = 90, .target = Other},
                {.effect = HighCritical, .bp = 70, .type = Normal, .accuracy = 100, .target = Other},
                {.effect = Substitute, .bp = 0, .type = Normal, .accuracy = 100, .target = Self},
                {.effect = Recoil, .bp = 50, .type = Normal, .accuracy = 100, .target = RandomFoe}
            } };
        }
        constexpr MoveData::MoveData move_data(Move move) {
            assert(move != Move::NullMove);
            return MoveData::DATA[move-1];
        }
	}
}