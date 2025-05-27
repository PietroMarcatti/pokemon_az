#pragma once
#include "species.hpp"
#include "moves.hpp"
#include "gen1battle.hpp"
#include <array>
#include <functional>
#include <iostream>

namespace pkmndriver {
	namespace Gen1 {
        struct PokemonState {
            Species species;
            uint8_t hp_bin;
            uint8_t status;
            std::array<Move, 4> moves;
            std::array<bool, 4> move_availability;
			PokemonState() : species(NullSpecies), hp_bin(0), status(0), moves({ NullMove, NullMove, NullMove, NullMove }), move_availability({ false, false, false, false }) {}
        };

        struct ActivePokemonState {
            Species species;
            uint8_t hp_bin;
            uint8_t status;
            std::array<bool, 8> volatiles; //binding, invulnerable, confusion, substitute, recharging, leech seed, lightscreen, reflect
            std::array<int8_t, 6> boost_bins;
            std::array<Move, 4> moves;
            std::array<bool, 4> move_availability;
			ActivePokemonState() : species(NullSpecies), hp_bin(0), status(0), volatiles({ false, false, false, false, false, false, false, false }),
                boost_bins({ 0, 0, 0, 0, 0, 0 }), moves({ NullMove, NullMove, NullMove, NullMove }), move_availability({ false, false, false, false }) {}
        };

        struct GameState {
            ActivePokemonState team1_active;
            ActivePokemonState team2_active;
            std::array<PokemonState, 6> team1;
            std::array<PokemonState, 6> team2;
			GameState() : team1_active(), team2_active(), team1({}), team2({}) {}
        };

        inline uint8_t compute_hp_bin(uint16_t current_hp, uint16_t max_hp) {
            double hp_left = static_cast<double>(current_hp) / max_hp;
            uint8_t hp_bin = 3;
            if (hp_left == 0) hp_bin = 0;
            else if (hp_left < 0.33) hp_bin = 1;
            else if (hp_left < 0.66) hp_bin = 2;
            return hp_bin;
        }

        inline uint8_t compute_status(uint8_t status) {
            if ((status & 0b111) > 0) return 2;
            if (status != 0) std::cout << "Status: " << (int)status << std::endl;
            return status;
        }

        inline std::array<int8_t, 6> compute_boost_bins(Slice& active) {
            std::array<int8_t, 6> boost_bins;
            std::array<int8_t, 6> boosts = { boost_atk(active), boost_def(active), boost_spe(active), boost_spec(active), boost_acc(active), boost_eva(active) };
            for (int i = 0; i < 6; i++) {
                switch (boosts[i]) {
                case -6: case -5: case -4: case -3: case -2: boost_bins[i] = -2; break;
                case -1: boost_bins[i] = -1; break;
                case 0: boost_bins[i] = 0; break;
                case 1: boost_bins[i] = 1; break;
                case 6: case 5: case 4: case 3: case 2: boost_bins[i] = 2; break;
                }
            }
            return boost_bins;
        }

        inline void side_gamestate(const Gen1Battle& battle, GameState& game_state, bool s1, RevealedSideMask mask) {
            Slice team = s1 ? side1(&battle) : side2(&battle);
            Slice act = active(team);
            Slice ord = order(team);
            std::array<PokemonState, 6>& team_array = s1 ? game_state.team1 : game_state.team2;
            for (int i = 0; i < 6; i++) {
                Slice p = pokemon(team, i);
                team_array[i] = {
                    mask.pokemon_masks[i].species ? species(p) : NullSpecies,
                    compute_hp_bin(current_hp(p), hitpoints(p)),
                    compute_status(status(p)),
                    {mask.pokemon_masks[i].moves[0] ? move_1_id(p) : NullMove, mask.pokemon_masks[i].moves[1] ? move_2_id(p) : NullMove,
                    mask.pokemon_masks[i].moves[2] ? move_3_id(p) : NullMove, mask.pokemon_masks[i].moves[3] ? move_4_id(p) : NullMove},
                    {move_1_pp(p) > 0, move_2_pp(p) > 0, move_3_pp(p) > 0, move_4_pp(p) > 0}
                };
            }
            ActivePokemonState& active_state = s1 ? game_state.team1_active : game_state.team2_active;
            uint8_t act_pos = ord[0] - 1;
            Slice active_slice = pokemon(team, act_pos);
            active_state.species = species_act(act);
            active_state.hp_bin = compute_hp_bin(current_hp(active_slice), hitpoints(act));
            active_state.status = compute_status(status(active_slice));
            active_state.volatiles = { binding(active_slice), invulnerable(active_slice), confusion(active_slice), substitute(active_slice),
                                           recharging(active_slice), leechSeed(active_slice), lightScreen(active_slice), reflect(active_slice) };
            active_state.boost_bins = compute_boost_bins(active_slice);
            active_state.moves = { mask.pokemon_masks[act_pos].moves[0] ? move_1_id(active_slice) : NullMove, mask.pokemon_masks[act_pos].moves[1] ? move_2_id(active_slice) : NullMove,
                                mask.pokemon_masks[act_pos].moves[2] ? move_3_id(active_slice) : NullMove, mask.pokemon_masks[act_pos].moves[3] ? move_4_id(active_slice) : NullMove };
            active_state.move_availability = { move_1_pp(active_slice) > 0, move_2_pp(active_slice) > 0, move_3_pp(active_slice) > 0, move_4_pp(active_slice) > 0 };
        }

        inline GameState perfect_knowledge_gamestate(Gen1Battle& battle) {
            GameState game_state;
            RevealedSideMask mask(true);
            side_gamestate(battle, game_state, true, mask);
            side_gamestate(battle, game_state, false, mask);
            return game_state;
        }

		/* TODO
		GameState imperfect_knowledge_gamestate(Gen1Battle& battle, bool p1_perspective) {
			GameState game_state;
			RevealedSideMask mask(true);
			side_gamestate(battle, game_state, true, mask);
			side_gamestate(battle, game_state, false, mask);
			return game_state;
		}*/
	}
}

namespace std {
    template <>
    struct hash<pkmndriver::Gen1::PokemonState> {
        size_t operator()(const pkmndriver::Gen1::PokemonState& p) const {
            size_t h = 0;
            h ^= std::hash<uint8_t>{}(p.species) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<uint8_t>{}(p.hp_bin) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<uint8_t>{}(p.status) + 0x9e3779b9 + (h << 6) + (h >> 2);
            for (int i = 0; i < 4; i++) {
                h ^= std::hash<uint8_t>{}(p.moves[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<bool>{}(p.move_availability[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }

            return h;
        }
    };

    template<>
    struct hash<pkmndriver::Gen1::ActivePokemonState> {
        size_t operator()(const pkmndriver::Gen1::ActivePokemonState& p) const {
            size_t h = 0;
            h ^= std::hash<uint8_t>{}(p.species) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<uint8_t>{}(p.hp_bin) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<uint8_t>{}(p.status) + 0x9e3779b9 + (h << 6) + (h >> 2);
            for (int i = 0; i < 8; i++) {
                h ^= std::hash<bool>{}(p.volatiles[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            for (int i = 0; i < 6; i++) {
                h ^= std::hash<int8_t>{}(p.boost_bins[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            for (int i = 0; i < 4; i++) {
                h ^= std::hash<uint8_t>{}(p.moves[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<bool>{}(p.move_availability[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };

    template <>
    struct hash<pkmndriver::Gen1::GameState> {
        size_t operator()(const pkmndriver::Gen1::GameState& gs) const {
            size_t h = 0;
            h ^= std::hash<pkmndriver::Gen1::ActivePokemonState>{}(gs.team1_active) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<pkmndriver::Gen1::ActivePokemonState>{}(gs.team2_active) + 0x9e3779b9 + (h << 6) + (h >> 2);
            for (int i = 0; i < 6; i++) {
                h ^= std::hash<pkmndriver::Gen1::PokemonState>{}(gs.team1[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<pkmndriver::Gen1::PokemonState>{}(gs.team2[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}
