//Abstract agent definition. Currently not used

#pragma once

#include "generation.hpp"
#include "pkmn.h"

namespace pkmndriver {
	template<Gen gen, typename GameState, typename Choice>
	struct Agent {
		pkmn_player player=PKMN_PLAYER_P1;
		GameState* gamestate = nullptr;
		virtual Choice make_choice(pkmn_result result) = 0;
	};

	using Gen1Agent = Agent<Gen::RBY, pkmn_gen1_battle, pkmn_choice> ;
}