#pragma once

#include "gen1pokemon.hpp"

namespace pkmndriver {
	namespace Gen1 {
        template<Gen gen>
        struct Side;

        template<>
        struct Side<Gen::RBY> {
            std::array<uint8_t, 184> bytes{ { 0 } };
            constexpr Side() {}

            template<typename ConT>
            constexpr Side(const ConT& side) {
                init(side);
            }

            template<typename ConT>
            constexpr void init(const ConT& pmember) {
                // braindamage or genius???
                for (int i = 0; i < 24; ++i) {
                    *(bytes.begin() + i) = *(pmember[0].bytes.begin() + i);
                    *(bytes.begin() + i + 24) = *(pmember[1].bytes.begin() + i);
                    *(bytes.begin() + i + 48) = *(pmember[2].bytes.begin() + i);
                    *(bytes.begin() + i + 72) = *(pmember[3].bytes.begin() + i);
                    *(bytes.begin() + i + 96) = *(pmember[4].bytes.begin() + i);
                    *(bytes.begin() + i + 120) = *(pmember[5].bytes.begin() + i);
                }

                // set active pokemon as first pokemon on the team
                // stats
                bytes[144] = pmember[0].bytes[0];
                bytes[145] = pmember[0].bytes[1];
                bytes[146] = pmember[0].bytes[2];
                bytes[147] = pmember[0].bytes[3];
                bytes[148] = pmember[0].bytes[4];
                bytes[149] = pmember[0].bytes[5];
                bytes[150] = pmember[0].bytes[6];
                bytes[151] = pmember[0].bytes[7];
                bytes[152] = pmember[0].bytes[8];
                bytes[153] = pmember[0].bytes[9];
                // species
                bytes[154] = pmember[0].bytes[21];
                // types
                bytes[155] = pmember[0].bytes[22];
                // boosts
                bytes[156] = 0;
                bytes[157] = 0;
                bytes[158] = 0;
                // zero padding
                bytes[159] = 0;
                // volatiles
                bytes[160] = 0;
                bytes[161] = 0;
                bytes[162] = 0;
                bytes[163] = 0;
                bytes[164] = 0;
                bytes[165] = 0;
                bytes[166] = 0;
                bytes[167] = 0;
                // moves
                bytes[168] = pmember[0].bytes[10];
                bytes[169] = pmember[0].bytes[11];
                bytes[170] = pmember[0].bytes[12];
                bytes[171] = pmember[0].bytes[13];
                bytes[172] = pmember[0].bytes[14];
                bytes[173] = pmember[0].bytes[15];
                bytes[174] = pmember[0].bytes[16];
                bytes[175] = pmember[0].bytes[17];

                bytes[176] = 1;
                bytes[177] = 2;
                bytes[178] = 3;
                bytes[179] = 4;
                bytes[180] = 5;
                bytes[181] = 6;
            }
            constexpr auto begin() {
                return bytes.begin();
            }
            constexpr auto end() {
                return bytes.end();
            }
            constexpr auto cbegin() const {
                return bytes.cbegin();
            }
            constexpr auto cend() const {
                return bytes.cend();
            }
            constexpr auto size() const {
                return bytes.size();
            }
        };
        using Gen1Side = Side<Gen::RBY>;
        template<typename ConT>
        Gen1Side make_side(ConT side) {
            return Gen1Side(side);
        }

        inline Gen1Side make_side(const Gen1Poke& p1,
                                  const Gen1Poke& p2,
                                  const Gen1Poke& p3,
                                  const Gen1Poke& p4,
                                  const Gen1Poke& p5,
                                  const Gen1Poke& p6) {
            return Gen1Side(std::array<Gen1Poke,6>{p1, p2, p3, p4, p5, p6});
        }

        inline Gen1Side make_side(Gen1Poke team[6]) {
            return Gen1Side(std::array<Gen1Poke,6>{team[0], team[1], team[2], team[3], team[4], team[5]});
        }
	}
}