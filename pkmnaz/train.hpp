#include <torch/torch.h>
#include <filesystem>
#include <omp.h>
#include "gen1battle.hpp"
#include "mcts.hpp"
#include "pkmn.h"

void save_checkpoint(const std::string& path, torch::nn::Module& model, torch::optim::Optimizer& optimizer, int epoch, float loss) {
	torch::serialize::OutputArchive archive;
	model.save(archive);
	optimizer.save(archive);
	archive.write("epoch", torch::tensor(epoch));
	archive.write("loss", torch::tensor(loss));
	archive.save_to(path);
}

bool load_checkpoint(const std::string& path, torch::nn::Module& model, torch::optim::Optimizer* optimizer, int& epoch_out, float& loss_out) {
	if (!std::filesystem::exists(path)) return false;
	torch::serialize::InputArchive archive;
	archive.load_from(path);
	model.load(archive);
	if (optimizer) {
		optimizer->load(archive);
	}

	torch::Tensor epoch_tensor, loss_tensor;
	archive.read("epoch", epoch_tensor);
	archive.read("loss", loss_tensor);
	epoch_out = epoch_tensor.item<int>();
	loss_out = loss_tensor.item<float>();
	return true;
}

std::pair<torch::Tensor, torch::Tensor> loss_fn(torch::Tensor policy_pred, torch::Tensor policy_target,
	torch::Tensor value_pred, torch::Tensor value_target) {

	auto policy_log_probs = torch::log_softmax(policy_pred, 1);
	auto policy_loss = torch::nn::functional::cross_entropy(
		policy_pred,
		policy_target
	);

	auto value_loss = torch::nn::functional::smooth_l1_loss(value_pred.squeeze(), value_target);

	return {policy_loss, value_loss};
}


std::vector<pkmn_choice> pkmn_choices = { 0, 1, 10, 14, 18, 22, 26, 5, 9, 13, 17 };


std::pair<std::vector<std::tuple<std::vector<float>, std::vector<float>, int>>,int> self_play_game(int game_index, PokemonAZNet* model=nullptr) {
	Gen1Battle battle = generateRandomBattle(game_index);
	MCTS mcts;
	battle.init();
	battle.play_turn(0, 0);

	std::vector<std::tuple<std::vector<float>, std::vector<float>, int>> game_data;

	while (!pkmn_result_type(battle.result)) {
		// --- Player 1 (p1_perspective = true)
		std::vector<float> state_p1 = extract_game_state(battle.battle_,true);
		std::vector<float> policy_p1 = mcts.get_action_distribution(battle, true, model);
		int choice_p1 = sample_with_weights(pkmn_choices, policy_p1);

		// --- Player 2 (p1_perspective = false)
		std::vector<float> state_p2 = extract_game_state(battle.battle_,false);
		std::vector<float> policy_p2 = mcts.create_action_distribution(false, mcts.compute_beta(mcts.t));
		int choice_p2 = sample_with_weights(pkmn_choices, policy_p2);

		// --- Play turn
		battle.play_turn(choice_p1, choice_p2);

		// --- Save data
		game_data.emplace_back(std::move(state_p1), std::move(policy_p1), 0);
		game_data.emplace_back(std::move(state_p2), std::move(policy_p2), 0);
	}

	// --- Game result to reward
	int reward = 0;
	switch (battle.result) {
	case 1: reward = 1; break;
	case 2: reward = -1; break;
	default: reward = 0;
	}

	for (auto& record : game_data) {
		std::get<2>(record) = reward;
	}
	//print_battle_result(battle.battle_, battle.result);

	return {game_data,turn(battle.battle_)};
}


void train_model(PokemonAZNet& model,
	std::vector<std::tuple<std::vector<float>, std::vector<float>, float>>& training_data,
	int epochs = 5, float lr = 0.0003) {

	model->train();
	torch::optim::Adam optimizer(model->parameters(), lr);

	for (int epoch = 0; epoch < epochs; ++epoch) {
		auto start = std::chrono::high_resolution_clock::now();
		float total_policy=0.0, total_value = 0.0, total_loss=0.0;
		const int batch_size = 64;
		for (size_t i = 0; i<training_data.size(); i+=batch_size) {
			auto end = std::min(i+batch_size, training_data.size());
			auto batch = std::vector(training_data.begin()+i, training_data.begin()+end);
			std::vector<torch::Tensor> states, policy_targets, value_targets;
			for (const auto& [state_vec, policy_vec, value_scalar] : batch) {
				states.push_back(torch::tensor(state_vec,torch::kFloat32));
				policy_targets.push_back(torch::tensor(policy_vec,torch::kFloat32));
				value_targets.push_back(torch::tensor(value_scalar,torch::kFloat32));

			}
			auto state_batch = torch::stack(states);
			auto policy_target_batch= torch::stack(policy_targets);
			auto value_target_batch = torch::stack(value_targets);

			optimizer.zero_grad();
			auto [policy_pred, value_pred] = model->forward(state_batch);
			// 1. Mask invalid actions
			auto mask = (policy_target_batch != 0.0f).to(torch::kBool);
			auto masked_policy_pred = policy_pred.masked_fill(~mask, -1e12);  // Mask BEFORE softmax

			// 2. Compute log-probabilities
			auto policy_log_probs = torch::softmax(masked_policy_pred, 1);

			// 3. Debug: Print log_softmax output (should NOT have -1e9)
			auto [policy_loss, value_loss] = loss_fn(policy_log_probs, policy_target_batch, value_pred, value_target_batch);
			auto loss = policy_loss+0.5*value_loss;
			loss.backward();
			optimizer.step();

			total_policy += policy_loss.item<float>();
			total_value += value_loss.item<float>();
			total_loss += policy_loss.item<float>();
			total_loss += value_loss.item<float>();
		}
		int divider = training_data.size() / batch_size+ (training_data.size() % batch_size) > 0;
		float avg_loss = total_loss / divider;
		float avg_policy = total_policy / divider;
		float avg_value = total_value / divider;
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "Epoch " << epoch + 1 << ", Policy Loss: " << avg_policy<< ", Value Loss: " << avg_value << "\tEpoch time: " << duration.count()/1000000.0 << " seconds"<< std::endl;;
	}
	torch::save(model, "checkpoints/latest.pt");
}

pkmn_choice model_choose_move(Gen1Battle battle, PokemonAZNet& model, pkmn_player player){
	pkmn_choice choices[9];
	auto state = torch::tensor(extract_game_state(battle.battle_,true), torch::kFloat32).unsqueeze(0);
	auto [logits, _1] = model->forward(state);
	pkmn_choice num_1 = player == PKMN_PLAYER_P1 ? battle.get_choices_p1(choices) : battle.get_choices_p2(choices);
	std::vector<int64_t> choice_vector;
	for (int j = 0; j < num_1; ++j) {
		choice_vector.push_back(choice_to_index(choices[j]));
	}
	auto squeezed_logits = logits.squeeze(0);
	auto sampled = sample_from_logits(squeezed_logits, choice_vector);
	return index_to_choice(sampled);
}

bool evaluate_models(PokemonAZNet& new_model, PokemonAZNet& best_model, int num_games = 25) {
	new_model->eval();
	best_model->eval();
	int new_wins = 0, best_wins = 0, draws = 0;

	for (int i = 0; i < num_games; ++i) {
		Gen1Battle battle = generateRandomBattle(i);
		battle.init();
		battle.play_turn(0, 0);
		pkmn_choice choices1[9], choices2[9];
		int turn = 0;
		while (!pkmn_result_type(battle.result)) {
			auto c1 = model_choose_move(battle, new_model, PKMN_PLAYER_P1);
			auto c2 = model_choose_move(battle, best_model, PKMN_PLAYER_P2);
			battle.play_turn(c1, c2);
		}

		switch (battle.result) {
		case 1: new_wins++; break;
		case 2: best_wins++; break;
		default: draws++;
		}
	}

	std::cout << "New: " << new_wins << ", Best: " << best_wins << ", Draws: " << draws << "\n";
	return new_wins > best_wins*1.2f;
}

float model_vs_random(PokemonAZNet& model, int num_games = 100) {
	model->eval();
	int new_wins = 0, best_wins = 0, draws = 0;
	for (int i = 0; i < num_games; ++i) {
		Gen1Battle battle = generateRandomBattle(i);
		battle.init();
		battle.play_turn(0, 0);
		pkmn_choice choices1[9], choices2[9];

		while (!pkmn_result_type(battle.result)) {
			pkmn_choice c1 = model_choose_move(battle,model,PKMN_PLAYER_P1);
			uint8_t num_1 = battle.get_choices_p1(choices1);
			uint8_t num_2 = battle.get_choices_p2(choices2);
			pkmn_choice c2 = choices2[(uint64_t)pkmn_psrng_next(&battle.random) * num_2 / 0x100000000];
			
			bool found1=false, found2=false;
			for (int j=0; j<num_1 && !found1; j++) {
				found1= choices1[j] == c1;
			}
			for (int j=0; j<num_2 && !found2; j++) {
				found2= choices2[j] == c2;
			}
			if (!found1 && !found2) {
				std::cout<<"Player 1 valid moves were: \n";
				for (int j=0; j<num_1; j++) {
					std::cout<<choices1[j]<<" ";
				}
				std::cout<<std::endl<<"Selected move: "<<c1<<"\n";
				std::cout<<"Player 2 valid moves were: \n";
				for (int j=0; j<num_2; j++) {
					std::cout<<choices2[j]<<" ";
				}
				std::cout<<"\nSelected move: "<<c2<<std::endl;
			}
			battle.play_turn(c1, c2);
		}
		switch (battle.result) {
		case 1: new_wins++; break;
		case 2: best_wins++; break;
		default: draws++;
		}
	}

	std::cout << "Model: " << new_wins << ", Random: " << best_wins << ", Draws: " << draws << "\n";
	return static_cast<float>(new_wins) / num_games;
}

void model_vs_mcts(PokemonAZNet& model, int num_games = 100) {
	model->eval();
	int new_wins = 0, best_wins = 0, draws = 0;

	#pragma omp parallel for
	for (int i = 0; i < num_games; ++i) {
		Gen1Battle battle = generateRandomBattle(i);
		MCTS mcts = MCTS();
		battle.init();
		battle.play_turn(0, 0);
		pkmn_choice choices1[9], choices2[9];

		while (!pkmn_result_type(battle.result)) {
			pkmn_choice c1 = model_choose_move(battle, model, PKMN_PLAYER_P1);

			auto num_2 = battle.get_choices_p2(choices2);
			std::vector<float> c2_weights = mcts.get_action_distribution(battle, false);
			std::discrete_distribution<> c1_dist(c2_weights.begin(), c2_weights.end());
			auto c2 = index_to_choice(c1_dist(MCTSNode::rd));
			battle.play_turn(c1, c2);
		}
		switch (battle.result) {
		case 1: new_wins++; break;
		case 2: best_wins++; break;
		default: draws++;
		}
	}

	std::cout << "Model: " << new_wins << ", MCTS: " << best_wins << ", Draws: " << draws << "\n";
}

uint8_t mcts_vs_random(int battle_seed) {
	Gen1Battle battle;
	int total_turns = 0;
	auto start = std::chrono::high_resolution_clock::now();
	battle = generateRandomBattle(battle_seed);
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
	return battle.result;
	//std::cout << "Turn/second: " << total_turns / (duration.count() / 1000000.0) << " t/s\n";
}