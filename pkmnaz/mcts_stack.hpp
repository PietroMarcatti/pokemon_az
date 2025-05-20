#pragma once
#include "gen1battle.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm> 
#include <random>
#include <cmath>
#include <ranges>
#include <unordered_set>
#include <stack>
#include <numeric>
#include <stdexcept>
#include <cstddef>  // for size_t


namespace pkmndriver {
	namespace Gen1 {
        int total_new = 0;
        int total_delete = 0;

        inline float fast_sqrt(float x) {
            const float threehalfs = 1.5F;

            float x2 = x * 0.5F;
            float y = x;

            long i = *(long*)&y;

            i = 0x5f3759df - (i >> 1);
            y = *(float*)&i;

            y = y * (threehalfs - (x2 * y * y));
            return 1/y;
        }

        struct PairHash {
            template <typename T1, typename T2>
            std::size_t operator()(const std::pair<T1, T2>& pair) const {
                return std::hash<T1>{}(pair.first) ^ (std::hash<T2>{}(pair.second) << 1);
            }
        };
        
        std::stack<int> free_nodes;
        int pop_called = 0;
        int push_called = 0;

        struct MCTSNode {
            pkmn_gen1_battle battle;
			pkmn_result result;
            MCTSNode* parent;
            std::vector<std::pair<pkmn_choice, pkmn_choice>> actions;
            std::unordered_map<std::pair<pkmn_choice, pkmn_choice>, MCTSNode*, PairHash> children;
            unsigned int visits = 0;
            float wins = 0.0;
            unsigned int id = 0;
            static std::mt19937 rd;

            MCTSNode() :id(10000) {};

            MCTSNode(const pkmn_gen1_battle& bat,const pkmn_result res, unsigned int id, MCTSNode* parent = nullptr) : battle(bat), result(res), parent(parent), id(id) {
                total_new++;
				uint8_t p1_moves = pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P1, pkmn_result_p1(result), c1, PKMN_CHOICES_SIZE);
                uint8_t p2_moves = pkmn_gen1_battle_choices(&battle, PKMN_PLAYER_P2, pkmn_result_p2(result), c2, PKMN_CHOICES_SIZE);
                actions.reserve(p1_moves * p2_moves);
                for (int i = 0; i < p1_moves; ++i) {
                    for (int j = 0; j < p2_moves; ++j) {
                        actions.emplace_back(c1[i], c2[j]);
                    }
                }
                children.clear();
                children.reserve(p1_moves * p2_moves);
            }

            bool is_fully_expanded() const {
                return children.size() == actions.size();
            }
            
            void free_subtree() {
                push_called++;
                free_nodes.push(id);
                for (auto [action, child] : children) {
                    child->free_subtree();
                }
            }
            
            private:
                pkmn_choice c1[9];
                pkmn_choice c2[9];
        };

        std::mt19937 MCTSNode::rd(std::random_device{}());
        constexpr size_t MAX_NODES = 10000;

        struct MCTS {
            MCTSNode* root;
            pkmn_psrng random;
            int iterations;
            float exploration_constant = 1.414f;
            int t;
            uint8_t sim_num;
            
            MCTSNode node_pool[MAX_NODES];

            MCTS():root(nullptr),random() {
                pop_called = 0;
                push_called = 0;
                iterations = 2000;
                sim_num = 4;
                exploration_constant = 1.414f;
            }

            inline float compute_beta(int x) {
                if (x <= 100) return 25;
                if (x >= 250) return 8;

                float k = 0.0307f;
                return 8 + 17 * exp(-k * (x - 100));
            }


            std::vector<float> get_action_distribution(const Gen1Battle& battle, bool p1) {
                t = *std::bit_cast<uint16_t*>(battle.battle_.bytes + 368);
				random = battle.random;
                /*node_count = 0;
                root = &(node_pool[node_count]);
                node_pool[node_count++] = MCTSNode(battle.battle_, battle.result, node_id_counter);
                node_id_counter++;*/
                
                if (root==nullptr || pkmn_result_type(battle.result)) {
                    //std::cout << "Era vuota" << "\n";
                    reset_node_pool();
                    int node_id = free_nodes.top();
                    node_pool[node_id] = MCTSNode(battle.battle_, battle.result, node_id);
                    root = &node_pool[node_id];
                    pop_called++;
                    free_nodes.pop();
                }
                else {
                    std::pair<pkmn_choice, pkmn_choice> last_action = battle.last_moves;
                    if (root->children.contains(last_action)/* && actions_match(battle)*/) {
						MCTSNode* new_root = root->children[last_action];
                        root->children.erase(last_action);
                        root->free_subtree();
						root = new_root;
                        root->parent = nullptr;
                    }
                    else {
                        //std::cout << "Tocca farla di nuovo" << "\n";
                        root->free_subtree();
                        int node_id = free_nodes.top();
                        node_pool[node_id] = MCTSNode(battle.battle_, battle.result, node_id);
						root = &node_pool[node_id];
                        pop_called++;
                        free_nodes.pop();
                    }
                }

                for (int i = 0; i < iterations; ++i) {
                    MCTSNode* node = root;
                    // Selection
                    while (node && node->is_fully_expanded() && !node->children.empty()) {
                        node = select_best_uct_child(node,p1);
                    }
                    // Expansion
                    if (node && !node->is_fully_expanded()) {
                        node = expand(node);
                    }
                    
                    // Simulation
                    float result = 0.0f;
                    if (node) {
                        result = simulate(node, random);
                    }
                    else if (pkmn_result_type(root->result)) {
                        result = (pkmn_result_type(root->result) == PKMN_RESULT_WIN) ? 1.0f : -1.0f;
                    }
                    // Backpropagation
                    backpropagate(node, result);
                }
                if (t < 60) {

                }
                return create_action_distribution(p1,compute_beta(t));
            }

            std::vector<float> create_action_distribution(bool p1, float beta = 8.0f) {

                float best_win_rate = p1 ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
                std::unordered_map<pkmn_choice, std::pair<float, int>> win_rates;
                for (const auto& [action_pair, child] : root->children) {
                    pkmn_choice our_move = p1 ? action_pair.first : action_pair.second;
                    if (win_rates.contains(our_move)) {
                        win_rates.at(our_move).first += child->wins;
                        win_rates.at(our_move).second += child->visits;
                    }
                    else
                        win_rates[our_move] = { static_cast<float>(child->wins) , child->visits };
                }
                std::vector<float> exp_values = { 0,0,0,0,0,0,0,0,0,0,0 };
                for (const auto& [move, data] : win_rates) {
                    float win_rate = data.first / data.second; // Compute average win rate
                    if (!p1) win_rate = -win_rate; // Flip the perspective for player 2

                    float exp_val = std::exp(beta * win_rate);
                    switch (move) {
                        case 0: exp_values[0] = exp_val; break;
                        case 1: exp_values[1] = exp_val; break;
                        case 10: exp_values[2] = exp_val; break;
                        case 14: exp_values[3] = exp_val; break;
                        case 18: exp_values[4] = exp_val; break;
                        case 22: exp_values[5] = exp_val; break;
                        case 26: exp_values[6] = exp_val; break;
                        case 5: exp_values[7] = exp_val; break;
                        case 9: exp_values[8] = exp_val; break;
                        case 13: exp_values[9] = exp_val; break;
                        case 17: exp_values[10] = exp_val; break;
                    }
                }
                float sum_exp = std::accumulate(exp_values.begin(), exp_values.end(), 0.0f);
                for (auto& val : exp_values) {
                    val = val / sum_exp;
                }
                return exp_values;
            }
        private:
            static std::unordered_map<unsigned int, float> log_cache;

            /*bool actions_match(const Gen1Battle& battle) {
                std::unordered_set<std::pair<pkmn_choice, pkmn_choice>, PairHash> actions;
                pkmn_choice c1[9];
                pkmn_choice c2[9];
                uint8_t p1_moves = pkmn_gen1_battle_choices(&battle.battle_, PKMN_PLAYER_P1, pkmn_result_p1(battle.result), c1, PKMN_CHOICES_SIZE);
                uint8_t p2_moves = pkmn_gen1_battle_choices(&battle.battle_, PKMN_PLAYER_P2, pkmn_result_p2(battle.result), c2, PKMN_CHOICES_SIZE);
                for (int i = 0; i < p1_moves; ++i) {
                    for (int j = 0; j < p2_moves; ++j) {
                        actions.emplace(c1[i], c2[j]);
                    }
                }
                return actions == root->actions;
            }*/

            void reset_node_pool() {
                while (!free_nodes.empty()) {
                    pop_called++;
                    free_nodes.pop();
                }
                for (int n = MAX_NODES - 1; n >=0 ; n--) {
                    push_called++;
                    free_nodes.push(n);
                }
            }

            MCTSNode* select_best_uct_child(MCTSNode* node,bool p1) const {
                float best_value = p1 ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
                MCTSNode* best_child = nullptr;
                for (const auto child : node->children | std::views::values) {
                    float eq_res = 0;
                    if (!log_cache.contains(node->visits))
                        log_cache[node->visits] = std::log(node->visits + 1.0f);
                    eq_res = fast_sqrt(log_cache.at(node->visits) / (child->visits + 1e-6));
                    float uct_value = (child->wins / (child->visits + 1e-6)) + exploration_constant * eq_res;
                    if ((p1 && (uct_value > best_value)) || (!p1 && (uct_value < best_value))) {
                        best_value = uct_value;
                        best_child = child;
                    }
                }
                return best_child;
            }

            MCTSNode* expand(MCTSNode* node) {
                for (const auto& action_pair : node->actions) {
                    if (!node->children.contains(action_pair)) {
                        pkmn_gen1_battle new_battle(node->battle);
                        pkmn_result res = pkmn_gen1_battle_update(&new_battle, action_pair.first, action_pair.second, nullptr);
                        
                        if (free_nodes.empty()) {
                            std::cout<<"Stop, stack finito\n";
                        }
                        int node_id = free_nodes.top();
                        pop_called++;
                        node_pool[node_id] = MCTSNode(new_battle, res, node_id, node);
                        node->children[action_pair] = &node_pool[node_id];
                        free_nodes.pop();
                        return node->children[action_pair];
                    }
                }
                return nullptr;
            }
            
            float simulate(MCTSNode* node, pkmn_psrng& random) {
                float total = 0;
                
                for (int i = 0; i < sim_num; i++) {
                    pkmn_gen1_battle temp_battle(node->battle);
                    pkmn_result temp_result = node->result;

                    while (!pkmn_result_type(temp_result)) {
                        pkmn_choice p1_choices[9];
                        pkmn_choice p2_choices[9];
                        uint8_t p1_moves = pkmn_gen1_battle_choices(&temp_battle, PKMN_PLAYER_P1, pkmn_result_p1(temp_result), p1_choices, PKMN_CHOICES_SIZE);
                        uint8_t p2_moves = pkmn_gen1_battle_choices(&temp_battle, PKMN_PLAYER_P2, pkmn_result_p2(temp_result), p2_choices, PKMN_CHOICES_SIZE);

                        pkmn_choice move1 = 0;
                        pkmn_choice move2 = 0;

                        move1 = p1_choices[(uint64_t)((pkmn_psrng_next(&random) * (uint64_t)p1_moves) / 0x100000000)];
                        move2 = p2_choices[(uint64_t)((pkmn_psrng_next(&random) * (uint64_t)p2_moves) / 0x100000000)];
                        temp_result = pkmn_gen1_battle_update(&temp_battle, move1, move2, nullptr);
                    }

                    total += (pkmn_result_type(temp_result) == PKMN_RESULT_WIN ? 1.0f : -1.0f);
                }
                return total / sim_num;
            }

            void backpropagate(MCTSNode* node, float result) {
                MCTSNode* temp = node;
				while (temp !=nullptr) {
                    temp->visits++;
                    temp->wins += result;
                    temp = temp->parent;
				}
            }
        };
        std::unordered_map<unsigned int, float> MCTS::log_cache;
	}
}