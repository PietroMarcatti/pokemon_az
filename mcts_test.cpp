#include "mcts_no_model.hpp"
#include "pkmndriver.hpp"
#include <chrono>
using namespace pkmndriver::Gen1;

int main() {
	const auto& sets = gen1RandBatsSets;
	MCTS::initialize_log_cache();
	for (int i = 0; i < 10; i++) {
		const auto& sets = gen1RandBatsSets;
		Gen1Battle battle;
		int total_turns = 0;
		auto start = std::chrono::high_resolution_clock::now();
		battle = generateRandomBattle(354);
		battle.init();
		MCTS mcts = MCTS();
		pkmn_choice c1 = 0, c2 = 0;
		pkmn_choice choices[9];
		pkmn_result result;
		uint8_t num_choices = 0;
		uint16_t t;

		battle.play_turn(c1, c2);
		while (!pkmn_result_type(battle.result)) {
			std::vector<float> weights = mcts.get_action_distribution(battle, true);
			std::discrete_distribution<> dist(weights.begin(), weights.end());
			c1 = index_to_choice(dist(MCTSNode::rd));
			num_choices = pkmn_gen1_battle_choices(&battle.battle_, PKMN_PLAYER_P2, pkmn_result_p2(battle.result), choices, 9);

			c2 = choices[(uint64_t)pkmn_psrng_next(&battle.random) * num_choices / 0x100000000];

			t = turn(battle.battle_);
			battle.play_turn(c1, c2);
		}
		total_turns += t;
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		//std::cout << battle_seed + 1 << " Execution time: " << duration.count() / 1000000.0 << "s : \t";
		//battle.print_result();
	}
	
	return 0;
}