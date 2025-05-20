#pragma once

#include <bit>
#include <cassert>
#include <iostream>
#include <fstream>

#include "pkmn.h"
#include "rangeview.hpp"
#include "agent.hpp"
#include "status.hpp"
#include "gen1side.hpp"

// defines a function FUNC that returns the value of 
// state.bytes[START] interpreted as type TYPE
#define SINGLE_BATTLE_VIEW(FUNC, TYPE, START)               \
    inline constexpr TYPE FUNC(pkmn_gen1_battle& state)     \
    {                                                       \
        return *std::bit_cast<TYPE*>(state.bytes + START);  \
    }                                                       \

// defines a function FUNC that returns the value of
// side.begin() + START interpreted as type TYPE
#define SINGLE_SLICE_VIEW(FUNC, TYPE, START)                   \
    inline TYPE FUNC(Slice& view)                    \
    {                                                          \
       return *reinterpret_cast<TYPE*>(view.begin() + START);  \
    }                                                          \

// returns iterators as SLICE struct beginning with FROM
// and ending with TO
#define SLICE(FUNC, FROM, TO)                             \
    inline constexpr Slice FUNC(pkmn_gen1_battle& state)  \
    {                                                     \
        auto first = std::begin(state.bytes) + FROM;      \
        auto last  = std::begin(state.bytes) + TO;        \
        return Slice(first, last);                        \
    }                                                     \
    /*
    inline constexpr Slice FUNC(Gen1Battle& battle)       \
    {                                                     \
        auto first = battle.begin() + FROM;               \
        auto last  = battle.begin() + TO;                 \
        return Slice(first, last);                        \
    }                                                     \
    */

    // slices a Slice into smaller Subslice
#define SUB_SLICE(FUNC, FROM, TO)             \
    inline constexpr Slice FUNC(Slice& team)  \
    {                                         \
        return Slice(team.begin() + FROM,     \
                     team.begin() + TO);      \
    }                                         \

// returns a bool for if certain values are set in Volatile (active)
#define VOLATILE_BOOL(FUNC, PLACE)                                              \
    inline constexpr bool FUNC(Slice& volatiles)                                \
    {                                                                           \
        std::uint64_t vol = *std::bit_cast<std::uint64_t*>(volatiles.begin());  \
        return vol & PLACE ? true : false;                                      \
    }                                                                           \

// returns the value of volatile features
#define VOLATILE_RANGE(FUNC, PLACE, SHIFT)                                      \
    inline constexpr std::uint64_t FUNC(Slice& volatiles)                       \
    {                                                                           \
        std::uint64_t vol = *std::bit_cast<std::uint64_t*>(volatiles.begin());  \
        return (vol & PLACE) >> SHIFT;                                          \
    }

namespace pkmndriver {
    namespace Gen1 {
        using Slice = RangeView<std::uint8_t*>;
        SLICE(side1, 0, 184)
            SLICE(side2, 184, 368)
            // slice pokemon of a team
            SUB_SLICE(pokemon1, 0, 24)
            SUB_SLICE(pokemon2, 24, 48)
            SUB_SLICE(pokemon3, 48, 72)
            SUB_SLICE(pokemon4, 72, 96)
            SUB_SLICE(pokemon5, 96, 120)
            SUB_SLICE(pokemon6, 120, 144)

            inline Slice pokemon(Slice& team, uint8_t p_num) {
            switch (p_num) {
            case 0: return pokemon1(team);
            case 1: return pokemon2(team);
            case 2: return pokemon3(team);
            case 3: return pokemon4(team);
            case 4: return pokemon5(team);
            case 5: return pokemon6(team);
                default: return pokemon1(team);
            }
        }
        // a pokemons moveset information
        SINGLE_SLICE_VIEW(move_1_id, Move, 10)
            SINGLE_SLICE_VIEW(move_1_pp, std::uint8_t, 11)
            SINGLE_SLICE_VIEW(move_2_id, Move, 12)
            SINGLE_SLICE_VIEW(move_2_pp, std::uint8_t, 13)
            SINGLE_SLICE_VIEW(move_3_id, Move, 14)
            SINGLE_SLICE_VIEW(move_3_pp, std::uint8_t, 15)
            SINGLE_SLICE_VIEW(move_4_id, Move, 16)
            SINGLE_SLICE_VIEW(move_4_pp, std::uint8_t, 17)
            // Pokemons status
            SINGLE_SLICE_VIEW(status, Status, 20)
            SINGLE_SLICE_VIEW(species, Species, 21)
            SINGLE_SLICE_VIEW(types, std::uint8_t, 22)
            SINGLE_SLICE_VIEW(level, std::uint8_t, 23)
            // get stats of any pokemon
            // only displacements that are the same for active and others

#define TWO_BYTE_SLICE_VIEW(FUNC, TYPE, FROM) \
        inline constexpr TYPE FUNC(const Slice& view) \
        {                                             \
            return static_cast<TYPE>(view[FROM+1] << 8 | view[FROM]);    \
        }                                             \

            TWO_BYTE_SLICE_VIEW(hitpoints, std::uint16_t, 0)
            TWO_BYTE_SLICE_VIEW(attack, std::uint16_t, 2)
            TWO_BYTE_SLICE_VIEW(defense, std::uint16_t, 4)
            TWO_BYTE_SLICE_VIEW(speed, std::uint16_t, 6)
            TWO_BYTE_SLICE_VIEW(special, std::uint16_t, 8)
            // Pokemons current HP
            TWO_BYTE_SLICE_VIEW(current_hp, std::uint16_t, 18)
            // get species of pokemon
            SINGLE_SLICE_VIEW(species_act, Species, 10)
            SINGLE_SLICE_VIEW(types_act, std::uint8_t, 11)
            // either sides active pokemon
            SUB_SLICE(active, 144, 176)
            // HACKY AF
            inline constexpr Type type1(Slice& types) {
            return std::bit_cast<Type>(static_cast<std::uint8_t>((*(types.begin()) & 0xf0)));
        }
        inline constexpr Type type2(Slice& types) {
            return std::bit_cast<Type>(static_cast<std::uint8_t>((*(types.begin()) & 0x0f)));
        }
        inline constexpr std::int8_t boost_atk(Slice& active) {
            return (*(active.begin() + 12) & 0xf0) >> 4;
        }
        inline constexpr std::int8_t boost_def(Slice& active) {
            return *(active.begin() + 12) & 0x0f;
        }
        inline constexpr std::int8_t boost_spe(Slice& active) {
            return (*(active.begin() + 13) & 0xf0) >> 4;
        }
        inline constexpr std::int8_t boost_spec(Slice& active) {
            return *(active.begin() + 13) & 0x0f;
        }
        inline constexpr std::int8_t boost_acc(Slice& active) {
            return (*(active.begin() + 14) & 0xf0) >> 4;
        }
        inline constexpr std::int8_t boost_eva(Slice& active) {
            return *(active.begin() + 14) & 0x0f;
        }
        // get volatiles from active pokemon
        SUB_SLICE(volatiles, 16, 24)

            // reading volatiles bits
            // TODO TEST AF
            static constexpr std::uint64_t state_range = (((std::uint64_t)-1 << 40) - 1) ^ (((std::uint64_t)1 << 24) - 1);
        static constexpr std::uint64_t sub_range = (((std::uint64_t)-1 << 48) - 1) ^ (((std::uint64_t)1 << 40) - 1);
        static constexpr std::uint64_t transform_range = (((std::uint64_t)-1 << 52) - 1) ^ (((std::uint64_t)1 << 48) - 1);
        static constexpr std::uint64_t disabled_dur_range = (((std::uint64_t)-1 << 56) - 1) ^ (((std::uint64_t)1 << 52) - 1);
        static constexpr std::uint64_t disabled_mov_range = (((std::uint64_t)-1 << 59) - 1) ^ (((std::uint64_t)1 << 56) - 1);
        static constexpr std::uint64_t toxic_range = ((std::uint64_t)-1) ^ (((std::uint64_t)1 << 59) - 1);

        VOLATILE_BOOL(bide, 0b000000000000000001)
            VOLATILE_BOOL(thrashing, 0b000000000000000010)
            VOLATILE_BOOL(multiHit, 0b000000000000000100)
            VOLATILE_BOOL(flinch, 0b000000000000001000)
            VOLATILE_BOOL(charging, 0b000000000000010000)
            VOLATILE_BOOL(binding, 0b000000000000100000)
            VOLATILE_BOOL(invulnerable, 0b000000000001000000)
            VOLATILE_BOOL(confusion, 0b000000000010000000)
            VOLATILE_BOOL(mist, 0b000000000100000000)
            VOLATILE_BOOL(focusEnergy, 0b000000001000000000)
            VOLATILE_BOOL(substitute, 0b000000010000000000)
            VOLATILE_BOOL(recharging, 0b000000100000000000)
            VOLATILE_BOOL(rage, 0b000001000000000000)
            VOLATILE_BOOL(leechSeed, 0b000010000000000000)
            VOLATILE_BOOL(toxic, 0b000100000000000000)
            VOLATILE_BOOL(lightScreen, 0b001000000000000000)
            VOLATILE_BOOL(reflect, 0b010000000000000000)
            VOLATILE_BOOL(transform, 0b100000000000000000)
            VOLATILE_RANGE(state, state_range, 24)
            VOLATILE_RANGE(substitute_hp, sub_range, 40)
            VOLATILE_RANGE(transform_id, transform_range, 48)
            VOLATILE_RANGE(disabled_dur, disabled_dur_range, 52)
            VOLATILE_RANGE(disabled_move, disabled_mov_range, 56)
            VOLATILE_RANGE(toxic_turns, toxic_range, 59)

#undef VOLATILE_RANGE
#undef VOLATILE_BOOL

            // active mons moveset information
            SINGLE_SLICE_VIEW(move_1_id_act, Move, 24)
            SINGLE_SLICE_VIEW(move_1_pp_act, std::uint8_t, 25)
            SINGLE_SLICE_VIEW(move_2_id_act, Move, 26)
            SINGLE_SLICE_VIEW(move_2_pp_act, std::uint8_t, 27)
            SINGLE_SLICE_VIEW(move_3_id_act, Move, 28)
            SINGLE_SLICE_VIEW(move_3_pp_act, std::uint8_t, 29)
            SINGLE_SLICE_VIEW(move_4_id_act, Move, 30)
            SINGLE_SLICE_VIEW(move_4_pp_act, std::uint8_t, 31)
            // order of a team
            SUB_SLICE(order, 176, 182)

#undef SUB_SLICE

            // last moves
            SINGLE_SLICE_VIEW(last_selected_move, std::uint8_t, 182)
            SINGLE_SLICE_VIEW(last_used_move, std::uint8_t, 183)

#undef SINGLE_SLICE_VIEW

            SINGLE_BATTLE_VIEW(turn, std::uint16_t, 368)
            SINGLE_BATTLE_VIEW(last_damage, std::uint16_t, 370)

#undef SINGLE_BATTLE_VIEW

            inline uint8_t options_p1_move(pkmn_gen1_battle& battle, pkmn_choice options[]) {
            return pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P1, PKMN_CHOICE_MOVE, options, PKMN_CHOICES_SIZE);
        }
        inline uint8_t options_p2_move(pkmn_gen1_battle& battle, pkmn_choice options[]) {
            return pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P2, PKMN_CHOICE_MOVE, options, PKMN_CHOICES_SIZE);
        }
        inline uint8_t options_p1_switch(pkmn_gen1_battle& battle, pkmn_choice options[]) {
            return pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P1, PKMN_CHOICE_SWITCH, options, PKMN_CHOICES_SIZE);
        }
        inline uint8_t options_p2_switch(pkmn_gen1_battle& battle, pkmn_choice options[]) {
            return pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P2, PKMN_CHOICE_SWITCH, options, PKMN_CHOICES_SIZE);
        }

        ///
        /// functions to write bytes in pkmn_gen1_battle
        ///
#define SET_SIDE(FUNC, FROM) \
        inline constexpr void FUNC(pkmn_gen1_battle& battle, std::array<std::uint8_t, 184> side)  \
        {                                                                                         \
            auto it_b = std::begin(battle.bytes) + FROM;                                          \
            auto it_e = std::begin(battle.bytes) + 184 + FROM;                                    \
            auto it_s = side.begin();                                                             \
                                                                                                  \
            for(; it_b != it_e; ++it_b, ++it_s)                                                   \
            {                                                                                     \
                *it_b = *it_s;                                                                    \
            }                                                                                     \
        }                                                                                         \

        SET_SIDE(set_side1, 0);
        SET_SIDE(set_side2, 184);

#undef SET_SIDE

#define SET_POKEMON_SLICE(FUNC, FROM, TO)                                       \
        inline constexpr void FUNC(const Slice& side, const Gen1Poke& pokemon)          \
        {                                                                               \
            auto it_s = side.begin() + FROM;                                            \
            auto it_e = side.begin() + TO;                                              \
            auto it_p = pokemon.bytes.begin();                                          \
                                                                                        \
            for(;it_s != it_e; ++it_s, ++it_p)                                          \
            {                                                                           \
                *it_s = *it_p;                                                          \
            }                                                                           \
        }                                                                               \

        SET_POKEMON_SLICE(set_pokemon1, 0, 24);
        SET_POKEMON_SLICE(set_pokemon2, 24, 48);
        SET_POKEMON_SLICE(set_pokemon3, 48, 72);
        SET_POKEMON_SLICE(set_pokemon4, 72, 96);
        SET_POKEMON_SLICE(set_pokemon5, 96, 120);
        SET_POKEMON_SLICE(set_pokemon6, 120, 144);

#undef SET_POKEMON_SLICE

        // set byte in pokemon
#define SET_POKEMON_BYTE(FUNC, BYTE)                              \
        inline constexpr void FUNC(Slice& pokemon, std::uint8_t set_byte) \
        {                                                                 \
            pokemon[BYTE] = set_byte;                                     \
        }                                                                 \

        SET_POKEMON_BYTE(set_move1, 10);
        SET_POKEMON_BYTE(set_pp1, 11);
        SET_POKEMON_BYTE(set_move2, 12);
        SET_POKEMON_BYTE(set_pp2, 13);
        SET_POKEMON_BYTE(set_move3, 14);
        SET_POKEMON_BYTE(set_pp3, 15);
        SET_POKEMON_BYTE(set_move4, 16);
        SET_POKEMON_BYTE(set_pp4, 17);
        SET_POKEMON_BYTE(set_current_hp, 18);
        SET_POKEMON_BYTE(set_status, 20);
        SET_POKEMON_BYTE(set_species, 21);
        SET_POKEMON_BYTE(set_level, 23);

#undef SET_POKEMON_BYTE

        inline constexpr void set_types(Slice& pokemon, Type type1, Type type2)
        {
            pokemon[22] = static_cast<uint8_t>(type1 | (type2 << 4));
        }

#define SET_STAT(FUNC, FROM)                                    \
        inline constexpr void FUNC(Slice& pokemon, std::uint16_t stat)  \
        {                                                               \
            pokemon[FROM]     = static_cast<uint8_t>(stat);             \
            pokemon[FROM + 1] = static_cast<uint8_t>(stat >> 8);                              \
        }                                                               \

        SET_STAT(set_hp, 0);
        SET_STAT(set_atk, 2);
        SET_STAT(set_def, 4);
        SET_STAT(set_spe, 6);
        SET_STAT(set_spc, 8);

#undef SET_STAT

        inline constexpr void set_atk_boost(Slice& active, std::int8_t boost) {
            active[12] &= ~0xf0;
            active[12] |= boost << 4;
        }
        inline constexpr void set_def_boost(Slice& active, std::int8_t boost) {
            active[12] &= ~0xf;
            active[12] |= boost;
        }
        inline constexpr void set_spe_boost(Slice& active, std::int8_t boost) {
            active[13] &= ~0xf0;
            active[13] |= boost << 4;
        }
        inline constexpr void set_spec_boost(Slice& active, std::int8_t boost) {
            active[13] &= ~0xf;
            active[13] |= boost;
        }
        inline constexpr void set_acc_boost(Slice& active, std::int8_t boost) {
            active[14] &= ~0xf0;
            active[14] |= boost << 4;
        }
        inline constexpr void set_eva_boost(Slice& active, std::int8_t boost) {
            active[14] &= ~0xf;
            active[14] |= boost;
        }

#define SET_VOLATILE_BIT(FUNC, BYTE, SHIFT)                 \
        inline constexpr void FUNC(Slice& active, std::uint8_t set) \
        {                                                           \
            active[BYTE] |= set << SHIFT;                           \
        }                                                           \

        SET_VOLATILE_BIT(set_bide, 16, 0)
            SET_VOLATILE_BIT(set_thrashing, 16, 1)
            SET_VOLATILE_BIT(set_multiHit, 16, 2)
            SET_VOLATILE_BIT(set_flinch, 16, 3)
            SET_VOLATILE_BIT(set_charging, 16, 4)
            SET_VOLATILE_BIT(set_binding, 16, 5)
            SET_VOLATILE_BIT(set_invulnerable, 16, 6)
            SET_VOLATILE_BIT(set_confusion, 16, 7)
            SET_VOLATILE_BIT(set_mist, 17, 0)
            SET_VOLATILE_BIT(set_focusEnergy, 17, 1)
            SET_VOLATILE_BIT(set_substitute, 17, 2)
            SET_VOLATILE_BIT(set_recharge, 17, 3)
            SET_VOLATILE_BIT(set_rage, 17, 4)
            SET_VOLATILE_BIT(set_leechSeed, 17, 5)
            SET_VOLATILE_BIT(set_toxic, 17, 6)
            SET_VOLATILE_BIT(set_lightScreen, 17, 7)
            SET_VOLATILE_BIT(set_reflect, 18, 0)
            SET_VOLATILE_BIT(set_transform, 18, 1)

#undef SET_VOLATILE_BIT

            inline constexpr void set_seed(pkmn_gen1_battle& battle, std::uint64_t seed) {
            for (int i = 0; i < 8; ++i) {
                battle.bytes[376 + i] = static_cast<uint8_t>(seed & 0xFF);
                seed >>= 8;
            }
        }

        inline void print_battle_result(pkmn_gen1_battle& battle, pkmn_result result) {
            // Determine system endianness
            volatile uint32_t endian = 0x01234567;
            uint16_t turns = static_cast<uint16_t>((*((uint8_t*)(&endian))) == 0x67
                ? battle.bytes[368] | battle.bytes[369] << 8
                : battle.bytes[368] << 8 | battle.bytes[369]);

            if (turns > 900) {
                std::cout << "Error with battle" << std::endl;
            }
            // The result is from the perspective of P1
            switch (pkmn_result_type(result)) {
            case PKMN_RESULT_WIN:
                std::cout << "Battle won by Player A after " << turns << " turns\n";
                break;
            case PKMN_RESULT_LOSE:
                std::cout << "Battle won by Player B after " << turns << " turns\n";
                break;
            case PKMN_RESULT_TIE:
                std::cout << "Battle ended in a tie after " << turns << " turns\n";
                break;
            case PKMN_RESULT_ERROR:
                std::cout << "Battle encountered an error after " << turns << " turns\n";
                break;
            case PKMN_RESULT_NONE: default:
                assert(false);
            }
        }

        struct RevealedPokemonMask {
            bool species;
            std::array<bool, 4> moves;
            RevealedPokemonMask() :species(false), moves({ {false, false,false,false} }) {}
        };

        struct RevealedSideMask {
            std::array<RevealedPokemonMask, 6> pokemon_masks;

            RevealedSideMask() :pokemon_masks({}) {}
        };

        inline void print_battle_result(pkmn_gen1_battle& battle, pkmn_result result);
        template<Gen gen>
        struct Battle;


        template<>
        struct Battle<Gen::RBY> {
            Gen1Agent* p1 = nullptr; //i am not using these
            Gen1Agent* p2 = nullptr; //i am not using these
            alignas(8) pkmn_gen1_battle battle_ {};
            std::uint64_t seed = 0;
            pkmn_psrng random = {};
            pkmn_result result = PKMN_RESULT_NONE;
            std::pair<pkmn_choice, pkmn_choice> last_moves{};
            pkmn_gen1_battle_options options = pkmn_gen1_battle_options();

            ~Battle() = default;
            Battle() = default;
            Battle(const Battle& other) = default;
            Battle(const Gen1Side& side1, const Gen1Side& side2, std::uint64_t seed = 0) :
                seed(seed)
            {
                auto it_s1 = side1.cbegin();
                auto it_e1 = side1.cend();
                auto it_s2 = side2.cbegin();
                auto it_b = std::begin(battle_.bytes);

                for (; it_s1 != it_e1; ++it_s1, ++it_s2, ++it_b)
                {
                    *it_b = *it_s1;
                    *(it_b + 184) = *it_s2;
                }

                // turn
                battle_.bytes[368] = 0;
                battle_.bytes[369] = 0;
                // last_damage
                battle_.bytes[370] = 0;
                battle_.bytes[371] = 0;
                // last_selected_indexes
                battle_.bytes[372] = 0;
                battle_.bytes[373] = 0;
                battle_.bytes[374] = 0;
                battle_.bytes[375] = 0;

                for (int i = 0; i < 8; ++i)
                {
                    battle_.bytes[376 + i] = static_cast<uint8_t>(seed >> 8 * i);
                }
            }
            Battle(Gen1Agent* p1, Gen1Agent* p2, const Gen1Side& side1, const Gen1Side& side2, std::uint64_t seed = 0) :
                Battle(side1, side2, seed) {
                this->p1 = p1;
                this->p1->gamestate = &battle_;
                this->p1->player = PKMN_PLAYER_P1;
                this->p2 = p2;
                this->p2->gamestate = &battle_;
                this->p2->player = PKMN_PLAYER_P2;
            }

            pkmn_gen1_battle* operator()() {
                return &battle_;
            }

            const pkmn_gen1_battle* operator()() const {
                return &battle_;
            }

            constexpr auto begin() {
                return std::begin(battle_.bytes);
            }

            constexpr auto end() {
                return std::end(battle_.bytes);
            }

            constexpr auto cbegin() const {
                return std::cbegin(battle_.bytes);
            }

            constexpr auto cend() const {
                return std::cend(battle_.bytes);
            }

            constexpr auto size() const {
                return std::size(battle_.bytes);
            }

            constexpr auto bytes() const {
                return battle_.bytes;
            }

            void init() {
                pkmn_psrng_init(&random, seed);
                pkmn_gen1_battle_options_set(&options, NULL, NULL, NULL);
            }

            inline uint8_t get_choices_p1(pkmn_choice actions[9]) {
                return pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P1, pkmn_result_p1(result), actions, PKMN_CHOICES_SIZE);
            }

            inline uint8_t get_choices_p2(pkmn_choice actions[9]) {
                return pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P2, pkmn_result_p2(result), actions, PKMN_CHOICES_SIZE);
            }

            pkmn_choice choose_p1() {
                return p1->make_choice(result);
            }

            pkmn_choice choose_p2() {
                return p2->make_choice(result);
            }

            pkmn_result play_turn(pkmn_choice choice_p1, pkmn_choice choice_p2) {
                //update_revealed_info(choice_p1,true);
                //update_revealed_info(choice_p2, false);
                last_moves = { choice_p1, choice_p2 };
                this->result = pkmn_gen1_battle_update(&battle_, choice_p1, choice_p2, &options);
                pkmn_gen1_battle_options_set(&options, NULL, NULL, NULL);
                return this->result;
            }

            pkmn_result play_turn() {
                return play_turn(choose_p1(), choose_p2());
            }

            pkmn_result play_battle() {
                init();

                result = pkmn_gen1_battle_update(&battle_, PKMN_CHOICE_PASS, PKMN_CHOICE_PASS, NULL);
                this->p1->gamestate = &battle_;
                this->p2->gamestate = &battle_;
                while (!pkmn_result_type(result)) {
                    result = play_turn();
                }

                return result;
            }

            void print_result() {
                print_battle_result(this->battle_, this->result);
            }


            /*
            void update_revealed_info(pkmn_choice choice, const bool& p) {
                RevealedSideMask& mask = p ? revealed_masks.first : revealed_masks.second;
                uint8_t* ord = battle_.bytes + !p * 184 + 176;
                uint8_t pos = static_cast<uint8_t>(ord[0]-1);
                mask.pokemon_masks[pos].species = true;
                uint8_t switch_pos;
                if (choice > 1) {
                    switch (choice) {
                        case 5: mask.pokemon_masks[pos].moves[0] = true; break;
                        case 9: mask.pokemon_masks[pos].moves[1] = true; break;
                        case 13: mask.pokemon_masks[pos].moves[2] = true; break;
                        case 17: mask.pokemon_masks[pos].moves[3] = true; break;
                        case 10: case 14: case 18: case 22: case 26:
                            switch_pos = ord[(choice>>2)-1]-1;
                            mask.pokemon_masks[switch_pos].species = true; break;
                    }
                }
            }*/


        };
        using Gen1Battle = Battle<Gen::RBY>;

    }

}
#undef SLICE

