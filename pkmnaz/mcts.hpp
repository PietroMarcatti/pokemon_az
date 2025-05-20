#pragma once
#include "gen1battle.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm> 
#include <random>
#include <cmath>
#include <ranges>
#include <numeric>
#include <cstddef>  // for size_t
#include "model.hpp"

namespace pkmndriver {
    namespace Gen1 {

        inline pkmn_choice index_to_choice(int index) {
            switch (index) {
                case 0: return 0;
                case 1: return 1;
                case 2: return 10;
                case 3: return 14;
                case 4: return 18;
                case 5: return 22;
                case 6: return 26;
                case 7: return 5;
                case 8: return 9;
                case 9: return 13;
                case 10: return 17;
                default: return 0;
            }
        }

        inline pkmn_choice choice_to_index(int index) {
            switch (index) {
                case 0: return 0;
                case 1: return 1;
                case 10: return 2;
                case 14: return 3;
                case 18: return 4;
                case 22: return 5;
                case 26: return 6;
                case 5: return 7;
                case 9: return 8;
                case 13: return 9;
                case 17: return 10;
                default: return 0;
            }
        }

        inline float fast_sqrt(float x) {
            const float threehalfs = 1.5F;

            float x2 = x * 0.5F;
            float y = x;

            uint32_t i = std::bit_cast<uint32_t>(y);  // Safe float-to-int conversion
            i = 0x5f3759df - (i >> 1);
            y = std::bit_cast<float>(i);              // Safe int-to-float conversion

            y = y * (threehalfs - (x2 * y * y));
            return 1 / y;
        }

        struct PairHash {
            template <typename T1, typename T2>
            std::size_t operator()(const std::pair<T1, T2>& p) const {
                return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
            }
        };

        struct MCTSNode {
            pkmn_gen1_battle battle;
            pkmn_result result;
            MCTSNode* parent;
            std::vector<std::pair<pkmn_choice, pkmn_choice>> actions;
            std::unordered_map<std::pair<pkmn_choice, pkmn_choice>, std::unique_ptr<MCTSNode>, PairHash> children;
            unsigned int visits = 0;
            float wins = 0.0;
            static thread_local std::mt19937 rd;
            MCTSNode() = default;

            MCTSNode(const pkmn_gen1_battle& bat, const pkmn_result res, MCTSNode* parent = nullptr) : battle(bat), result(res), parent(parent) {
                uint8_t p1_moves = pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P1, pkmn_result_p1(result), c1, PKMN_CHOICES_SIZE);
                uint8_t p2_moves = pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P2, pkmn_result_p2(result), c2, PKMN_CHOICES_SIZE);
                actions.reserve(p1_moves * p2_moves);
                for (int i = 0; i < p1_moves; ++i)
                    for (int j = 0; j < p2_moves; ++j)
                        actions.emplace_back(c1[i], c2[j]);
            }

            bool is_fully_expanded() const {
                return children.size() == actions.size();
            }

        private:
            pkmn_choice c1[9];
            pkmn_choice c2[9];
        };

        thread_local std::mt19937 MCTSNode::rd(std::random_device{}());

        int move_to_index(pkmn_choice move) {
            switch (move) {
            case 0: return 0;
            case 1: return 1;
            case 10: return 2;
            case 14: return 3;
            case 18: return 4;
            case 22: return 5;
            case 26: return 6;
            case 5: return 7;
            case 9: return 8;
            case 13: return 9;
            case 17: return 10;
            default: return -1;
            }
        }

        struct MCTS {
            std::unique_ptr<MCTSNode> root;
            pkmn_psrng random;
            int iterations;
            float exploration_constant;
            int t;
            uint8_t sim_num;

            MCTS() :root(nullptr), random() {
                iterations = 2000;
                sim_num = 1;
                exploration_constant = 1.414f;
            }

            float compute_beta(int x) {
                if (x <= 100) return 25;
                if (x >= 250) return 8;

                float k = 0.0307f;
                return 8 + 17 * exp(-k * (x - 100));
            }

            std::vector<float> get_action_distribution(const Gen1Battle& battle, bool p1, PokemonAZNet* model=nullptr) {
                t = *std::bit_cast<uint16_t*>(battle.battle_.bytes + 368);
                random = battle.random;
                root = std::make_unique<MCTSNode>(battle.battle_, battle.result);
                /*
                if (root == nullptr || pkmn_result_type(battle.result)) {
                    root = std::make_unique<MCTSNode>(battle.battle_, battle.result);
                }
                else {
                    auto it = root->children.find(battle.last_moves);
                    if (it != root->children.end()) {
                        // Move the subtree out before deleting the rest
                        std::unique_ptr<MCTSNode> new_root = std::move(it->second);

                        // Important: disconnect parent pointer to avoid holding onto deleted memory
                        new_root->parent = nullptr;
                        root.reset();
                        // Now replace the tree root with the subtree
                        root = std::move(new_root);
                    }
                    else {
                        root = std::make_unique<MCTSNode>(battle.battle_, battle.result);
                    }
                }*/

                for (int i = 0; i < iterations; ++i) {
                    MCTSNode* node = root.get();
                    // Selection
                    while (node && node->is_fully_expanded() && !node->children.empty()) {
                        node = select_best_uct_child(node, p1);
                    }
                    // Expansion
                    if (node && !node->is_fully_expanded()) {
                        node = expand(node);
                    }

                    // Simulation
                    float result = 0.0f;
                    if (node) {
                        result = simulate(node, random, model);
                    }
                    else if (pkmn_result_type(root->result)) {
                        result = (pkmn_result_type(root->result) == PKMN_RESULT_WIN) ? 1.0f : -1.0f;
                    }
                    // Backpropagation
                    backpropagate(node, result);
                }
                auto policy = create_action_distribution(p1, compute_beta(t));
                return policy;
            }

            std::vector<float> create_action_distribution(bool p1, float beta = 8.0f) {

                std::unordered_map<pkmn_choice, std::pair<float, int>> win_rates;

                // Aggregate wins and visits for each unique move by P1 or P2 depending on the perspective
                for (const auto& [action_pair, child] : root->children) {
                    pkmn_choice our_move = p1 ? action_pair.first : action_pair.second;

                    auto& [total_wins, total_visits] = win_rates[our_move];
                    total_wins += child->wins;
                    total_visits += child->visits;
                }
                std::vector<float> exp_values(11, 0.0f);
                for (const auto& [move, data] : win_rates) {
                    int idx = move_to_index(move);
                    if (idx < 0 || idx >= 11) continue;

                    float win_rate = data.first / data.second;
                    if (!p1) win_rate = -win_rate; // Flip for P2's perspective

                    float exp_val = std::exp(beta * win_rate);
                    exp_values[idx] = exp_val;
                }
                float sum_exp = std::accumulate(exp_values.begin(), exp_values.end(), 0.0f);
                if (sum_exp > 0.0f) {
                    for (auto& val : exp_values) val /= sum_exp;
                }
                float sum = std::accumulate(exp_values.begin(), exp_values.end(), 0.0f);
                if (std::abs(sum - 1.0f) > 1e-3) {
                    std::cerr << "Policy target sum is off: " << sum << std::endl;
                }
                assert(exp_values.size() == 11 && "Exp values size mismatch");
                return exp_values;
            }

            static void initialize_log_cache() {
                log_cache.resize(100'000'001); // Include 0 to 100,000,000
                log_cache[0] = 0.0f; // Define as 0 or NaN/throw depending on needs

                for (size_t i = 1; i < log_cache.size(); ++i) {
                    log_cache[i] = std::log(static_cast<float>(i));
                }
            }
        private:
            static std::vector<float> log_cache;
            MCTSNode* select_best_uct_child(MCTSNode* node, bool p1) const {
                float best_value = p1 ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
                MCTSNode* best_child = nullptr;
                for (const auto& [_, child] : node->children) {
                    float eq_res = 0;
                    eq_res = fast_sqrt(log_cache[node->visits] / (child->visits + 1e-6f));
                    float uct_value = (child->wins / (child->visits + 1e-6)) + exploration_constant * eq_res;
                    if ((p1 && (uct_value > best_value)) || (!p1 && (uct_value < best_value))) {
                        best_value = uct_value;
                        best_child = child.get();
                    }
                }
                return best_child;
            }

            MCTSNode* expand(MCTSNode* node) {
                for (const auto& action_pair : node->actions) {
                    if (!node->children.contains(action_pair)) {
                        pkmn_gen1_battle new_battle(node->battle);
                        pkmn_result res = pkmn_gen1_battle_update(&new_battle, action_pair.first, action_pair.second, nullptr);
                        auto new_node = std::make_unique<MCTSNode>(new_battle, res, node);
                        MCTSNode* new_ptr = new_node.get();
                        node->children[action_pair] = std::move(new_node);
                        return new_ptr;
                    }
                }
                return nullptr;
            }

            float simulate(MCTSNode* node, pkmn_psrng& random, PokemonAZNet* model=nullptr) {
                float total = 0;
                for (int i = 0; i < sim_num; i++) {
                    pkmn_gen1_battle temp_battle(node->battle);
                    pkmn_result temp_result = node->result;
                    pkmn_choice p1_choices[9];
                    pkmn_choice p2_choices[9];
                    int t = 0;
                    while (!pkmn_result_type(temp_result)) {
                        int c1=0, c2=0;
                        if (model && t<=10) {
                            auto state_new = torch::tensor(extract_game_state(temp_battle,true), torch::kFloat32).unsqueeze(0);
                            auto state_best = torch::tensor(extract_game_state(temp_battle,false), torch::kFloat32).unsqueeze(0);

                            auto [logits_new, _1] = (*model)->forward(state_new);
                            auto [logits_best, _2] = (*model)->forward(state_best);

                            auto num_1 = pkmn_gen1_battle_choices(&temp_battle,PKMN_PLAYER_P1, pkmn_result_p1(temp_result),p1_choices,9);
                            auto num_2 = pkmn_gen1_battle_choices(&temp_battle,PKMN_PLAYER_P2, pkmn_result_p2(temp_result),p2_choices,9);
                            std::vector<int64_t> v1;
                            for (int j = 0; j < num_1; ++j) {
                                v1.push_back(choice_to_index(p1_choices[j]));
                            }
                            std::vector<int64_t> v2;
                            for (int j = 0; j < num_2; ++j) {
                                v2.push_back(choice_to_index(p2_choices[j]));
                            }

                            auto squeezed_logits_new = logits_new.squeeze(0);
                            auto squeezed_logits_best = logits_best.squeeze(0);

                            auto sampled_new = sample_from_logits(squeezed_logits_new, v1);
                            auto sampled_best = sample_from_logits(squeezed_logits_best, v2);

                            c1 = index_to_choice(sampled_new);
                            c2 = index_to_choice(sampled_best);
                        }else {
                            uint8_t p1_moves = pkmn_gen1_battle_choices(&temp_battle, PKMN_PLAYER_P1, pkmn_result_p1(temp_result), p1_choices, PKMN_CHOICES_SIZE);
                            uint8_t p2_moves = pkmn_gen1_battle_choices(&temp_battle, PKMN_PLAYER_P2, pkmn_result_p2(temp_result), p2_choices, PKMN_CHOICES_SIZE);

                            c1 = p1_choices[(uint64_t)((pkmn_psrng_next(&random) * (uint64_t)p1_moves) / 0x100000000)];
                            c2 = p2_choices[(uint64_t)((pkmn_psrng_next(&random) * (uint64_t)p2_moves) / 0x100000000)];
                        }
                        t++;
                        temp_result = pkmn_gen1_battle_update(&temp_battle, c1, c2, nullptr);
                    }
                    total += (pkmn_result_type(temp_result) == PKMN_RESULT_WIN ? 1.0f : -1.0f);
                }
                return total / sim_num;
            }

            void backpropagate(MCTSNode* node, float result) {
                MCTSNode* temp = node;
                while (temp != nullptr) {
                    temp->visits++;
                    temp->wins += result;
                    temp = temp->parent;
                }
            }
        };
        std::vector<float> MCTS::log_cache;
    }
}