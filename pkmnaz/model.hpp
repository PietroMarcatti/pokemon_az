#include <torch/torch.h>
#include <filesystem>
#include "pkmndriver.hpp"

using namespace pkmndriver::Gen1;

inline std::vector<double> decode_status(uint8_t num) {
    std::vector<double> result(7, 0.0);
    // Bit 7 (0x80) indicates either TOX or self-inflicted sleep (SLF)
    if (num & 0x80) {
        if (num == 0x88) {  // 0x80 | (1 << 3) = TOX (badly poisoned)
            result[6] = 1.0;
        }
        if ((num & 0x07) > 0) {  // Bits 0–2 = sleep duration
            result[5] = 1.0;       // Self-inflicted sleep
        }
    }
    if ((num & 0x07) > 0) {  // Regular sleep
        result[4] = 1.0;
    }
    if (num & (1 << 3)) {
        result[3] = 1.0;
    }
    if (num & (1 << 4)) {
        result[2] = 1.0;
    }
    if (num & (1 << 5)) {
        result[1] = 1.0;
    }
    if (num & (1 << 6)) {
        result[0] = 1.0;
    }
    return result;  // No status
}

inline void extract_perfect_pokemon_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, uint8_t side, uint8_t p_num, int offset) {
	RangeView<uint8_t*> team = side == 0 ? side1(battle_) : side2(battle_);
	RangeView<uint8_t*> p = pokemon(team, p_num);
	game_state[offset] = static_cast<double>(current_hp(p)) / hitpoints(p);
	if (std::isnan(game_state[offset]) || std::isinf(game_state[offset])) {
		std::cout<<"Inf or nan detected. Current hp was "<<current_hp(p)<<" and hitpoints were "<<hitpoints(p)<<std::endl;
		std::cout<<"Species was: "<<(int)species(p)<<std::endl;
		game_state[offset] = 0.56; // random placeholder
	}
	game_state[offset + 1] = static_cast<double>(attack(p)) / 274; //Normalization by the maximum value of the stat
	game_state[offset + 2] = static_cast<double>(defense(p)) / 335;
	game_state[offset + 3] = static_cast<double>(speed(p)) / 288;
	game_state[offset + 4] = static_cast<double>(special(p)) / 259;
	std::vector<double> move_type(15);
	move_type[move_data(move_1_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 5);
	game_state[offset + 20] = move_1_pp(p) > 0;
	game_state[offset + 21] = move_data(move_1_id(p)).bp/130.0;
	move_type[move_data(move_1_id(p)).type] = 0;
	move_type[move_data(move_2_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 22);
	game_state[offset + 37] = move_2_pp(p) > 0;
	game_state[offset + 38] = move_data(move_2_id(p)).bp/130.0;
	move_type[move_data(move_2_id(p)).type] = 0;
	move_type[move_data(move_3_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 39);
	game_state[offset + 54] = move_3_pp(p) > 0;
	game_state[offset + 55] = move_data(move_3_id(p)).bp/130.0;
	move_type[move_data(move_3_id(p)).type] = 0;
	move_type[move_data(move_4_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 56);
	game_state[offset + 71] = move_4_pp(p) > 0;
	game_state[offset + 72] = move_data(move_4_id(p)).bp/130.0;

	std::vector<double> st = decode_status((int)status(p));
	std::ranges::copy(st, game_state.begin() + offset + 73);

	move_type[move_data(move_4_id(p)).type] = 0;
	std::array<uint8_t, 2> types = get_type(species(p));
	move_type[types[0]] = 1;
	move_type[types[1]] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 80);
	game_state[offset + 95] = static_cast<double>(level(p)) / 100;
}

inline void extract_active_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, uint8_t side, bool p1_perspective, int offset) {
    RangeView<uint8_t*> team = (side == 0 ? side1(battle_) : side2(battle_));
    RangeView<uint8_t*> ord = order(team);
    uint8_t act_pos = ord[0] - 1;
    RangeView<uint8_t*> p = active(team);
    extract_perfect_pokemon_state(battle_,game_state, side, act_pos, offset);
    //binding, invulnerable, confusion, substitute, recharging, leech seed, lightscreen, reflect
    std::vector<bool> vol = { binding(p), invulnerable(p), confusion(p), substitute(p),recharging(p), leechSeed(p), lightScreen(p), reflect(p) };
    std::ranges::copy(vol, game_state.begin() + offset + 96);
    std::vector<float> boosts = { boost_atk(p) / 6.0f, boost_def(p) / 6.0f,boost_spe(p) / 6.0f, boost_spec(p) / 6.0f, boost_acc(p) / 6.0f, boost_eva(p) / 6.0f };
    std::ranges::copy(boosts, game_state.begin() + offset + 100);
}

inline void extract_side_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, uint8_t side, bool p1_perspective) {
    bool own_side = (p1_perspective && side == 0) || (!p1_perspective && side == 1);
    for (uint8_t i = 0; i < 6; i++) {
        if (own_side)
            extract_perfect_pokemon_state(battle_,game_state, side, i, 96 * i);
        else
            extract_perfect_pokemon_state(battle_,game_state, side, i, 686 + 96 * i); //Should actually be a different imperfect version
    }
    extract_active_state(battle_,game_state, side, p1_perspective, (1 - own_side) * 686 + 576);
}

inline std::vector<float> extract_game_state(pkmn_gen1_battle& battle_, bool p1_perspective) {
    std::vector<float> game_state(1372, 0.0f);
    if (p1_perspective) {
        extract_side_state(battle_,game_state, 0, true);
        extract_side_state(battle_,game_state, 1, true);
    }
    else {
        extract_side_state(battle_,game_state, 1, true);
        extract_side_state(battle_,game_state, 0, true);
    }
    assert(game_state.size() == 1372);
    return game_state;
}

struct PokemonAZNetImpl : torch::nn::Module {
	torch::nn::Linear fc1{ nullptr }, fc2{ nullptr }, fc3{ nullptr };
	torch::nn::Linear policy_head{ nullptr }, value_head{ nullptr };

	PokemonAZNetImpl(int64_t input_dim, int64_t output_dim) {
		fc1 = register_module("fc1", torch::nn::Linear(input_dim, 256));
		fc2 = register_module("fc2", torch::nn::Linear(256, 256));
		fc3 = register_module("fc3", torch::nn::Linear(256, 256));
		policy_head = register_module("policy_head", torch::nn::Linear(256, output_dim));
		value_head = register_module("value_head", torch::nn::Linear(256, 1));
	}

	std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor x) {
		x = torch::relu(fc1->forward(x));
		x = torch::relu(fc2->forward(x));
		auto policy_logits = policy_head->forward(fc3->forward(x));
		auto value = torch::tanh(value_head->forward(x));
		return { policy_logits, value };
	}
};
TORCH_MODULE(PokemonAZNet);

thread_local std::random_device rd;
thread_local std::mt19937 gen(rd());

template <typename T>
T sample_with_weights(const std::vector<T>& choices, const std::vector<float>& weights) {

	std::discrete_distribution<> dist(weights.begin(), weights.end());
	return T(choices[dist(gen)]);
}

template <typename T>
T sample_from_logits(const at::Tensor& logits, const std::vector<T>& choices) {
	auto filtered_logits = logits.index_select(0, torch::tensor(choices, torch::kLong));
	auto probs = torch::softmax(filtered_logits, 0);

	std::vector<float> prob_vec(probs.template data_ptr<float>(), probs.template data_ptr<float>() + probs.numel());
	return sample_with_weights(choices, prob_vec);
}


