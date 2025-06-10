#include "driver/src/gen1/gen1battle.hpp"
#include "pkmn.h"
#include "pkmnaz/mcts.hpp"
#include "pkmnaz/train.hpp"
#include <omp.h>
#include <chrono>
#include <iostream>
#include <fstream>

using namespace pkmndriver::Gen1;


float updateElo(float &eloA, float &eloB, float scoreA, int K=16) {
    float expectedA = 1.0f / (1.0f + std::pow(10.0f, (eloB - eloA)/400.0f));
    float delta = K * (scoreA - expectedA);
    return delta;
}

void evaluateModelsRoundRobin(std::vector<std::tuple<PokemonAZNet,int,float>>& model_history, int games_per_pair = 400) {
    std::mt19937 rng(std::time(nullptr));
    const int N = model_history.size();
	std::vector<float> deltas(model_history.size(),0.0f);
    int total_matches = (N * (N - 1) / 2) * games_per_pair;
    int progress = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            // Evaluate models before match
            std::get<0>(model_history[i])->eval();
            std::get<0>(model_history[j])->eval();

            int wins_i = 0, wins_j = 0;

            for (int g = 0; g < games_per_pair; ++g) {
                Gen1Battle battle = generateRandomBattle(g);
                battle.init();
				if(g%2 == 0){
					while (!battle.result) {
						int move_i = model_choose_move(battle, std::get<0>(model_history[i]), PKMN_PLAYER_P1);
						int move_j = model_choose_move(battle, std::get<0>(model_history[j]), PKMN_PLAYER_P2);
						battle.play_turn(move_i, move_j);
					}

					if (battle.result == 1) wins_i++;
					else if (battle.result == 2) wins_j++;
				}else{
					while (!battle.result) {
						
						int move_j = model_choose_move(battle, std::get<0>(model_history[j]), PKMN_PLAYER_P1);
						int move_i = model_choose_move(battle, std::get<0>(model_history[i]), PKMN_PLAYER_P2);
						battle.play_turn(move_j, move_i);
					}

					if (battle.result == 1) wins_j++;
					else if (battle.result == 2) wins_i++;
				}
                
                // Ties ignored or you can count them as 0.5 each
            }

            float score_i = static_cast<float>(wins_i) / games_per_pair;
            float delta = updateElo(std::get<2>(model_history[i]), std::get<2>(model_history[j]), score_i);
			deltas[i] += delta;
			deltas[j] -= delta;
        }
    }

    for (int i = 0; i < N; ++i) {
		std::get<2>(model_history[i]) += deltas[i];
        std::cout << "Iteration " << std::get<1>(model_history[i]) << " model elo: " << std::fixed << std::setprecision(2)
                  << std::get<2>(model_history[i]) << "\n";
    }
}

void writeEloCSV(const std::vector<std::tuple<PokemonAZNet,int,float>>& elos, const std::string& filename = "elo.csv") {
    std::ofstream file(filename); // Overwrites the file

    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    file << "iteration,elo\n";  // CSV header

    for (size_t i = 0; i < elos.size(); ++i) {
        file << std::get<1>(elos[i]) << "," << std::get<2>(elos[i]) << "\n";
    }

    file.close();
}

void save_weights(PokemonAZNet& model, const std::string& path){
	torch::serialize::OutputArchive archive;
	model->save(archive);  // Saves parameters/buffers
	archive.save_to(path);
}

void load_weights(PokemonAZNet& model, const std::string& path) {
	torch::serialize::InputArchive archive;
	archive.load_from(path);
	model->load(archive);  // Load parameters
}

int main() {
	auto sets = gen1RandBatsSets;
	MCTS::initialize_log_cache();
	std::mt19937 rng(std::time(nullptr));  // Random number generator
	int wins_counter = 0;
    int loss_counter = 0;
    int draws_counter = 0;

	std::array<std::tuple<int, int, int, int, int>,151> stats;
	std::array<double, 5> average_stats{0.0,0.0,0.0,0.0,0.0};

	for(int i = 0; i<1000; i++){
		Gen1Battle battle = generateRandomBattle(i);
		Slice team1 = side1(battle.battle_);
		Slice team2 = side2(battle.battle_);
		for(int i = 0; i<6; i++){
			Slice p1 = pokemon(team1,i);
			Slice p2 = pokemon(team2,i);
			std::get<0>(stats[species(p1)-1])=hitpoints(p1);
			std::get<1>(stats[species(p1)-1])=attack(p1);
			std::get<2>(stats[species(p1)-1])=defense(p1);
			std::get<3>(stats[species(p1)-1])=speed(p1);
			std::get<4>(stats[species(p1)-1])=special(p1);
			std::get<0>(stats[species(p2)-1])=hitpoints(p2);
			std::get<1>(stats[species(p2)-1])=attack(p2);
			std::get<2>(stats[species(p2)-1])=defense(p2);
			std::get<3>(stats[species(p2)-1])=speed(p2);
			std::get<4>(stats[species(p2)-1])=special(p2);
		}
	}
	for(int i = 0; i<stats.size();i++){
		average_stats[0]+=static_cast<double>(std::get<0>(stats[i]))/(stats.size()-5);
		average_stats[1]+=static_cast<double>(std::get<1>(stats[i]))/(stats.size()-5);
		average_stats[2]+=static_cast<double>(std::get<2>(stats[i]))/(stats.size()-5);
		average_stats[3]+=static_cast<double>(std::get<3>(stats[i]))/(stats.size()-5);
		average_stats[4]+=static_cast<double>(std::get<4>(stats[i]))/(stats.size()-5);
		
	}
	std::cout<<"{"<<average_stats[0]<<","<<average_stats[1]<<","<<average_stats[2]<<","<<average_stats[3]<<","<<average_stats[4]<<"}\n";

	std::array<int,15> type_dist{0};
	for(int i = 1; i<=151;i++){
		if(i == 10 || i == 11 || i == 13 || i==14 || i==129) continue;
		auto ty = get_type(Species(i+1));
		type_dist[ty[0]]+=1;
		if(ty[0] != ty[1]){
			type_dist[ty[1]]+=1;
		}
	}
	for(auto val : type_dist){
		std::cout<<static_cast<double>(val)/gen1RandBatsSets.size()<<", ";
	}
	std::cout<<"\n"<<std::flush;
	
	std::array<float,15> move_type{0.0f};
	float move_power = 0.0f;
	int attack_counter=0;
	for(int i = 0; i<gen1RandBatsSets.size();i++){
		std::array<float,15> pokemon_move_types{0.0f};
		for(auto move : gen1RandBatsSets[i].moves){
			auto data = move_data(move.first);
			pokemon_move_types[data.type]+=move.second;
			if(data.bp!=1){
				attack_counter++;
				move_power+= move.second*data.bp;
			}
		}
		for(int j=0;j<15;j++){
			move_type[j]+=pokemon_move_types[j]/gen1RandBatsSets[i].moves.size();
		}
	}
	for(auto val : move_type){
		std::cout<<static_cast<double>(val)/gen1RandBatsSets.size()<<", ";
	}
	std::cout<<"\n"<<std::flush;
	std::cout<<move_power/attack_counter<<"\n"<<std::flush;


	PokemonAZNet model(1372, 11);
	PokemonAZNet best_model(1372, 11);
	int sim_games = 100;
	

	if (!std::filesystem::exists("checkpoints")) std::filesystem::create_directory("checkpoints");
	if (std::filesystem::exists("checkpoints/latest.pt")) load_weights(model, "checkpoints/best.pt");
	else std::cout << "No latest checkpoint found, starting fresh.\n";
	if (std::filesystem::exists("checkpoints/best.pt")) load_weights(best_model, "checkpoints/best.pt");
	else std::cout << "No best checkpoint found, starting fresh.\n";

	std::vector<std::tuple<PokemonAZNet,int,float>> model_history;
	int latest_iter = 0;

    for (const auto& entry : std::filesystem::directory_iterator("checkpoints")) {
        if (entry.is_regular_file()) {
            // Get filename with extension, e.g., "165.txt"
            std::string filename = entry.path().filename().string();
			if(filename == "latest.pt" || filename == "best.pt") continue;
            // Remove the extension
            std::string stem = entry.path().stem().string(); // e.g., "165"
			PokemonAZNet previous(1372,11);
			load_weights(previous, "checkpoints/"+filename);
            model_history.push_back({previous,std::stoi(stem),1000});
			if(std::stoi(stem) > latest_iter)
				latest_iter = std::stoi(stem);
        }
    }

	std::sort(model_history.begin(), model_history.end(), [](const auto& a, const auto& b) {
        return std::get<1>(a) < std::get<1>(b);
    });

	if(model_history.size()>0){
		evaluateModelsRoundRobin(model_history);
		writeEloCSV(model_history);
	}

	omp_set_num_threads(10);
	for (int iteration = latest_iter; iteration < latest_iter+10000; ++iteration) {
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
		
		#pragma omp parallel
		{
			std::vector<std::tuple<std::vector<float>, std::vector<float>, float>> local_data;

			#pragma omp for nowait
			for (int i = 0; i < sim_games; ++i) {
				auto [game_data,turn_count] = self_play_game(i, nullptr);  // vector of turns
				local_data.insert(local_data.end(), game_data.begin(), game_data.end());

				int progress = ++progress_counter;
				std::cout << "\rProgress: " << progress << " / " << sim_games << "\tTurns played: "<< turn_count << "    " << std::flush;
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
		if (evaluate_models(model, best_model,400)) {
			model_vs_mcts(model, 50);
			std::cout << "New model better. Updating.\n";
			best_model = model;
			save_weights(model, "checkpoints/best.pt");
			save_weights(model, "checkpoints/"+std::to_string(iteration)+".pt");
			model_history.push_back({best_model,iteration,1000});
			if(model_history.size()>=5){
				evaluateModelsRoundRobin(model_history);
				writeEloCSV(model_history);
			}
		}
		float win_rate = model_vs_random(model, 1000);
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