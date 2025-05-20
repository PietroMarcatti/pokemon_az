#include <torch/torch.h>
#include <filesystem>
#include <omp.h>
#include "mcts.hpp"



torch::Tensor masked_softmax(const torch::Tensor& logits, const std::vector<bool>& mask) {
	torch::Tensor masked = logits.clone();
	for (size_t i = 0; i < mask.size(); ++i) {
		if (!mask[i]) {
			masked[i] = -std::numeric_limits<float>::infinity();
		}
	}
	return torch::softmax(masked, -1);
}

torch::Tensor cross_entropy_loss(const torch::Tensor& logits, const torch::Tensor& target) {
	return torch::nn::functional::cross_entropy(logits, target);
}

torch::Tensor mse_loss(const torch::Tensor& prediction, const torch::Tensor& target) {
	return torch::mse_loss(prediction.squeeze(), target);
}

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

int sample_weighted(const std::vector<float>& weights) {
	std::discrete_distribution<> dist(weights.begin(), weights.end());
	return dist(gen);
}

torch::Tensor loss_fn(torch::Tensor policy_pred, torch::Tensor policy_target,
	torch::Tensor value_pred, torch::Tensor value_target) {

	auto policy_log_probs = torch::log_softmax(policy_pred, 1);
	auto policy_loss = torch::nn::functional::kl_div(
		policy_log_probs,
		policy_target,
		torch::nn::functional::KLDivFuncOptions().reduction(torch::kBatchMean)
	);

	auto value_loss = torch::mse_loss(value_pred.squeeze(), value_target);
	return policy_loss + value_loss;
}



std::vector<pkmn_choice> pkmn_choices = { 0, 1, 10, 14, 18, 22, 26, 5, 9, 13, 17 };


std::vector<std::tuple<std::vector<float>, std::vector<float>, int>> self_play_game(int game_index, PokemonAZNet* model=nullptr) {
	Gen1Battle battle = generateRandomBattle(game_index);
	MCTS mcts;
	battle.init();
	battle.play_turn(0, 0);

	std::vector<std::tuple<std::vector<float>, std::vector<float>, int>> game_data;

	while (!pkmn_result_type(battle.result)) {
		// --- Player 1 (p1_perspective = true)
		std::vector<float> state_p1 = extract_game_state(battle.battle_,true);
		assert(state_p1.size() == 1372 && "State P1 has incorrect size");

		std::vector<float> policy_p1 = mcts.get_action_distribution(battle, true, model);
		assert(policy_p1.size() == pkmn_choices.size() && "Policy P1 size mismatch");

		int choice_p1 = sample_with_weights(pkmn_choices, policy_p1);

		// --- Player 2 (p1_perspective = false)
		std::vector<float> state_p2 = extract_game_state(battle.battle_,false);
		assert(state_p2.size() == 1372 && "State P2 has incorrect size");

		std::vector<float> policy_p2 = mcts.create_action_distribution(false, mcts.compute_beta(mcts.t));
		assert(policy_p2.size() == pkmn_choices.size() && "Policy P2 size mismatch");

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

	return game_data;
}


void train_model(PokemonAZNet& model,
	const std::vector<std::tuple<std::vector<float>, std::vector<float>, float>>& training_data,
	int epochs = 5, float lr = 0.001) {

	model->train();
	torch::optim::Adam optimizer(model->parameters(), lr);
	for (int epoch = 0; epoch < epochs; ++epoch) {
		auto start = std::chrono::high_resolution_clock::now();
		float total_loss = 0.0;
		for (const auto& [state_vec, policy_target_vec, value_target] : training_data) {
			assert(state_vec.size() == 1372);
			assert(policy_target_vec.size() == 11);
			assert(!std::isnan(value_target) && !std::isinf(value_target));

			assert(std::none_of(state_vec.begin(), state_vec.end(), [](float x){ return std::isnan(x) || std::isinf(x); }));
			assert(std::none_of(policy_target_vec.begin(), policy_target_vec.end(), [](float x){ return std::isnan(x) || std::isinf(x); }));
			auto state = torch::tensor(state_vec, torch::kFloat32).unsqueeze(0);
			auto policy_target = torch::tensor(policy_target_vec, torch::kFloat32).unsqueeze(0);
			auto value_target_tensor = torch::tensor( value_target , torch::kFloat32);

			optimizer.zero_grad();
			try {
				auto [policy_pred, value_pred] = model->forward(state);
				auto loss = loss_fn(policy_pred, policy_target, value_pred, value_target_tensor);
				loss.backward();
				optimizer.step();

				total_loss += loss.item<float>();
			}
			catch (const c10::Error& e) {
				std::cerr << "LibTorch error: " << e.msg() << std::endl;
				throw;
			}
		}
		float avg_loss = total_loss / training_data.size();
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "Epoch " << epoch + 1 << ", Loss: " << avg_loss << "\tEpoch time: " << duration.count()/1000000.0 << " seconds"<< std::endl;;
	}
	torch::save(model, "checkpoints/latest.pt");
}


bool evaluate_models(PokemonAZNet& new_model, PokemonAZNet& best_model, int num_games = 25) {
	new_model->eval();
	best_model->eval();
	int new_wins = 0, best_wins = 0, draws = 0;
	//omp_set_num_threads(10);
	//#pragma omp parallel for reduction(+:new_wins) reduction(+:draws) reduction(+:best_wins)
	for (int i = 0; i < num_games; ++i) {
		Gen1Battle battle = generateRandomBattle(i);
		battle.init();
		battle.play_turn(0, 0);
		pkmn_choice choices1[9], choices2[9];
		int turn = 0;
		while (!pkmn_result_type(battle.result)) {

			auto state_new = torch::tensor(extract_game_state(battle.battle_,true), torch::kFloat32).unsqueeze(0);
			auto state_best = torch::tensor(extract_game_state(battle.battle_,false), torch::kFloat32).unsqueeze(0);

			auto [logits_new, _1] = new_model->forward(state_new);
			auto [logits_best, _2] = best_model->forward(state_best);

			auto num_1 = battle.get_choices_p1(choices1);
			auto num_2 = battle.get_choices_p2(choices2);
			std::vector<int64_t> v1;
			for (int j = 0; j < num_1; ++j) {
				v1.push_back(choice_to_index(choices1[j]));
			}
			std::vector<int64_t> v2;
			for (int j = 0; j < num_2; ++j) {
				v2.push_back(choice_to_index(choices2[j]));
			}

			auto squeezed_logits_new = logits_new.squeeze(0);
			auto squeezed_logits_best = logits_best.squeeze(0);

			auto sampled_new = sample_from_logits(squeezed_logits_new, v1);
			auto sampled_best = sample_from_logits(squeezed_logits_best, v2);

			auto c1 = index_to_choice(sampled_new);
			auto c2 = index_to_choice(sampled_best);
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

	std::cout << "New: " << new_wins << ", Best: " << best_wins << ", Draws: " << draws << "\n";
	return new_wins > best_wins;
}

float test_against_random(PokemonAZNet& model, int num_games = 100) {
	model->eval();
	int new_wins = 0, best_wins = 0, draws = 0;

	for (int i = 0; i < num_games; ++i) {
		Gen1Battle battle = generateRandomBattle(i);
		battle.init();
		battle.play_turn(0, 0);
		pkmn_choice choices1[9], choices2[9];

		while (!pkmn_result_type(battle.result)) {
			auto state_new = torch::from_blob(extract_game_state(battle.battle_,true).data(), { 1, 1372 }).clone();

			auto [logits_new, _1] = model->forward(state_new);

			auto num_1 = battle.get_choices_p1(choices1);
			auto num_2 = battle.get_choices_p2(choices2);
			std::vector<int64_t> v1;
			for (int j = 0; j < num_1; ++j) {
				v1.push_back(choice_to_index(choices1[j]));
			}
			std::vector<int64_t> v2;
			for (int j = 0; j < num_2; ++j) {
				v2.push_back(choice_to_index(choices2[j]));
			}

			auto squeezed_logits_new = logits_new.squeeze(0);

			auto sampled_new = sample_from_logits(squeezed_logits_new, v1);

			auto c1 = index_to_choice(sampled_new);
			auto c2 = choices2[(uint64_t)pkmn_psrng_next(&battle.random) * num_2 / 0x100000000];
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

void test_against_mcts(PokemonAZNet& model, int num_games = 100) {
	model->eval();
	int new_wins = 0, best_wins = 0, draws = 0;

	for (int i = 0; i < num_games; ++i) {
		Gen1Battle battle = generateRandomBattle(i);
		MCTS mcts = MCTS();
		battle.init();
		battle.play_turn(0, 0);
		pkmn_choice choices1[9], choices2[9];

		while (!pkmn_result_type(battle.result)) {
			auto state_new = torch::from_blob(extract_game_state(battle.battle_,true).data(), { 1, 1372 }).clone();

			auto [logits_new, _1] = model->forward(state_new);

			auto num_1 = battle.get_choices_p1(choices1);
			auto num_2 = battle.get_choices_p2(choices2);
			std::vector<int64_t> v1;
			for (int j = 0; j < num_1; ++j) {
				v1.push_back(choice_to_index(choices1[j]));
			}
			std::vector<int64_t> v2;
			for (int j = 0; j < num_2; ++j) {
				v2.push_back(choice_to_index(choices2[j]));
			}

			auto squeezed_logits_new = logits_new.squeeze(0);

			auto sampled_new = sample_from_logits(squeezed_logits_new, v1);

			auto c1 = index_to_choice(sampled_new);
			std::vector<float> c2_weights = mcts.get_action_distribution(battle, false);
			std::discrete_distribution<> c1_dist(c2_weights.begin(), c2_weights.end());
			auto c2 = index_to_choice(c1_dist(MCTSNode::rd));
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

	std::cout << "Model: " << new_wins << ", MCTS: " << best_wins << ", Draws: " << draws << "\n";
}

void test_normal(int battle_seed) {
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
	battle.print_result();
	//std::cout << "Turn/second: " << total_turns / (duration.count() / 1000000.0) << " t/s\n";
}