#include "pkmnaz/train.hpp"

using namespace pkmndriver::Gen1;

int main() {
	const auto& sets = gen1RandBatsSets;
	MCTS::initialize_log_cache();
	for (int i = 0; i < 1; i++) {
		//auto game_data = self_play_game(i, nullptr);  // vector of turns
		test_normal(1);
	}

	PokemonAZNet model(1372, 11);
	PokemonAZNet best_model(1372, 11);
	int sim_games = 300;

	if (!std::filesystem::exists("checkpoints")) std::filesystem::create_directory("checkpoints");
	if (std::filesystem::exists("checkpoints/latest.pt")) torch::load(model, "checkpoints/latest.pt");
	else std::cout << "No latest checkpoint found, starting fresh.\n";
	if (std::filesystem::exists("checkpoints/best.pt")) torch::load(best_model, "checkpoints/best.pt");
	else std::cout << "No best checkpoint found, starting fresh.\n";

	//test_against_mcts(model, sim_games);

	for (int iteration = 0; iteration < 1000; ++iteration) {
		std::cout << "Iteration " << iteration + 1 << "\n";
		auto start = std::chrono::high_resolution_clock::now();
		std::cout << "\rProgress: " << 0 << " / " << sim_games << std::flush;
		std::vector<std::tuple<std::vector<float>, std::vector<float>, float>> training_data;
		std::mutex data_mutex;
		std::atomic<int> progress_counter = 0;

		/*for (int i = 0; i < sim_games; ++i) {
			auto game_data = self_play_game(i);  // returns a vector of turns
			training_data.insert(training_data.end(), game_data.begin(), game_data.end());

			std::cout << "\rProgress: " << (i + 1) << " / " << sim_games << std::flush;
		}*/
		best_model->eval();
		omp_set_num_threads(6);
		#pragma omp parallel
		{
			std::vector<std::tuple<std::vector<float>, std::vector<float>, float>> local_data;

			#pragma omp for nowait
			for (int i = 0; i < sim_games; ++i) {
				auto game_data = self_play_game(i, nullptr);  // vector of turns
				local_data.insert(local_data.end(), game_data.begin(), game_data.end());

				int progress = ++progress_counter;
				std::cout << "\rProgress: " << progress << " / " << sim_games << "\tTurns played: "<< game_data.size()/2 << std::flush;
			}

			// Critical section: safely merge thread-local data into global container
			std::lock_guard<std::mutex> lock(data_mutex);
			training_data.insert(training_data.end(), local_data.begin(), local_data.end());
		}
		std::cout << "\n";


		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		//std::cout << "Simulation time: " << duration.count() << " microseconds" << std::endl;

		train_model(model, training_data);
		//test_against_random(model, 100);
		//
		if (evaluate_models(model, best_model,100)) {
			test_against_mcts(model, 50);
			std::cout << "New model better. Updating.\n";
			best_model = model;
			torch::save(best_model, "checkpoints/best.pt");
		}
		float win_rate = test_against_random(model, 100);
		std::string name = "checkpoints/"+std::to_string(iteration)+"-"+std::to_string(win_rate)+".pt";
		torch::save(model, name);
	}
}

/*
*void save_battle(const Gen1Battle& battle, const std::string& filename) {
	std::ofstream out(filename, std::ios::binary);
	if (out) out.write(reinterpret_cast<const char*>(&battle), sizeof(battle));
}

bool load_battle(Gen1Battle& battle, const std::string& filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		in.read(reinterpret_cast<char*>(&battle), sizeof(battle));
		return true;
	}
	return false;
}

void check_health(Gen1Battle& battle) {
	Slice team1 = side1(battle.battle_);
	Slice team2 = side2(battle.battle_);
	Slice ord1 = order(team1);
	Slice ord2 = order(team2);
	std::array<uint16_t, 12> hp_values;
	for (int i = 0; i < 6; i++) {
		Slice p = pokemon(team1, i);
		hp_values[i] = current_hp(p);
	}
	for (int i = 0; i < 6; i++) {
		Slice p = pokemon(team2, i);
		hp_values[6 + i] = current_hp(p);
	}
	std::cout << "1: " << hp_values[ord1[0] - 1] << " 2: " << hp_values[ord1[1] - 1] << " 3: " << hp_values[ord1[2] - 1] << " 4: " << hp_values[ord1[3] - 1] << " 5: " << hp_values[ord1[4] - 1] << " 6: " << hp_values[ord1[5] - 1] << "\n";
	std::cout << "1: " << hp_values[6 + ord2[0] - 1] << " 2: " << hp_values[6 + ord2[1] - 1] << " 3: " << hp_values[6 + ord2[2] - 1] << " 4: " << hp_values[6 + ord2[3] - 1] << " 5: " << hp_values[6 + ord2[4] - 1] << " 6: " << hp_values[6 + ord2[5] - 1] << "\n";
	pkmn_choice choices[9];
	uint8_t my_num = pkmn_gen1_battle_choices(&battle.battle_, PKMN_PLAYER_P1, pkmn_result_p1(battle.result), choices, PKMN_CHOICES_SIZE);
	for (int move_n = 0; move_n < my_num; move_n++) {
		std::cout << move_n << ": " << (int)choices[move_n] << " ";
	}
	std::cout << "\n";
}
 */