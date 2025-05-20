#pragma once

#include "base_stats.hpp"
#include "generation.hpp"
#include "moves.hpp"
#include "species.hpp"
#include "types.hpp"

namespace pkmndriver {
	namespace Gen1 {
		template<Gen gen>
		struct Pokemon;

		inline constexpr std::uint16_t calc_other_stat(Pokemon<Gen::RBY>& poke, Stat stat_);
		inline constexpr std::uint16_t calc_hp(Pokemon<Gen::RBY>& poke);

		template<>
		struct Pokemon<Gen::RBY> {
			std::array<uint8_t, 24> bytes{ {0} };
			Pokemon() {
				bytes = { { 0 } };
			}
			Pokemon(Species p_species, std::array<Move,4> p_moves, uint8_t p_level = 100,
				std::array<std::uint8_t, 5> ev = { { 63, 63, 63, 63, 63 } }, std::array<std::uint8_t, 5> iv = { { 15, 15, 15, 15, 15 } }) {
				EV = ev;
				IV = iv;
				bytes[20] = 0; // status
				bytes[21] = p_species;
				bytes[23] = p_level;

				base = get_base_stats(bytes[21]);

				bytes[22] = static_cast<uint8_t>(get_type(p_species)[0] | (get_type(p_species)[1] << 4));

				bytes[10] = p_moves[0];
				bytes[11] = move_pp(p_moves[0]);
				bytes[12] = p_moves[1];
				bytes[13] = move_pp(p_moves[1]);
				bytes[14] = p_moves[2];
				bytes[15] = move_pp(p_moves[2]);
				bytes[16] = p_moves[3];
				bytes[17] = move_pp(p_moves[3]);

				calc_stats();

				// this sounds rough but we are killing None here before it had a chance to live
				if (p_species == NullSpecies) {
					bytes[18] = 0;
					bytes[19] = 0;
				}
			}
			constexpr void calc_stats() {
				bytes[0] = (uint8_t)calc_hp(*this);
				bytes[1] = static_cast<uint8_t>(calc_hp(*this) >> 8);
				if (bytes[0] == 0 && bytes[1]== 0) {
					std::cout<<"HP calcolata erroneamente"<<std::endl;
				}
				bytes[18] = (uint8_t)bytes[0];
				bytes[19] = bytes[1];
				bytes[2] = (uint8_t)calc_other_stat(*this, ATK);
				bytes[3] = static_cast<uint8_t>(calc_other_stat(*this, ATK) >> 8);
				bytes[4] = (uint8_t)calc_other_stat(*this, DEF);
				bytes[5] = static_cast<uint8_t>(calc_other_stat(*this, DEF) >> 8);
				bytes[6] = (uint8_t)calc_other_stat(*this, SPE);
				bytes[7] = static_cast<uint8_t>(calc_other_stat(*this, SPE) >> 8);
				bytes[8] = (uint8_t)calc_other_stat(*this, SPC);
				bytes[9] = static_cast<uint8_t>(calc_other_stat(*this, SPC) >> 8);
			}
			constexpr auto begin() { return bytes.begin(); }
			constexpr auto end() {return bytes.end();}
			constexpr auto cbegin() {return bytes.cbegin();}
			constexpr auto cend() { return bytes.cend(); }
			constexpr auto size() { return bytes.size(); }

			std::array<std::uint16_t,5> base{ };
			std::array<std::uint8_t, 5> EV{ { 63, 63, 63, 63, 63 } };
			std::array<std::uint8_t, 5> IV{ { 15, 15, 15, 15, 15 } };
		};

		using Gen1Poke = Pokemon<Gen::RBY>;
		inline constexpr std::uint16_t calc_hp(Pokemon<Gen::RBY>& poke) {
			std::uint16_t hp = static_cast<uint16_t>(poke.base[0] + poke.IV[0]);
			hp *= 2;
			hp += poke.EV[0];
			// multiply by level
			hp *= poke.bytes[23];
			hp /= 100;
			hp += poke.bytes[23] + 10;


			return hp;
		}
		inline constexpr std::uint16_t calc_other_stat(Pokemon<Gen::RBY>& poke, Stat stat_) {
			std::uint16_t stat = static_cast<uint16_t>(poke.base[stat_] + poke.IV[stat_]);
			stat *= 2;
			stat += poke.EV[stat_];
			// multiply by level
			stat *= poke.bytes[23];
			stat /= 100;
			stat += 5;

			return stat;
		}
	}
}