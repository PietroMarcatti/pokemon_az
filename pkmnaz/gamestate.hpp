#include "pkmndriver.hpp"

using namespace pkmndriver::Gen1;

float compute_hp_prior(RevealedSideMask mask){

}

std::map<std::string,int> game_state_sizes = {
    {"pokemon",96},
    {"side",686},
    {"inactives",576},
}

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

inline void extract_perfect_pokemon_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, pkmn_player player, uint8_t p_num, int offset) {
	RangeView<uint8_t*> team = player == PKMN_PLAYER_P1 ? side1(battle_) : side2(battle_);
	RangeView<uint8_t*> p = pokemon(team, p_num);
	game_state[offset] = static_cast<double>(current_hp(p)) / hitpoints(p);
    game_state[offset + 1] = static_cast<double>(hitpoints(p))/481; //Normalization by the maximum value of the stat
	game_state[offset + 2] = static_cast<double>(attack(p)) / 274; 
	game_state[offset + 3] = static_cast<double>(defense(p)) / 335;
	game_state[offset + 4] = static_cast<double>(speed(p)) / 288;
	game_state[offset + 5] = static_cast<double>(special(p)) / 259;
	std::vector<double> move_type(15);
	move_type[move_data(move_1_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 6);
	game_state[offset + 21] = move_1_pp(p) > 0;
	game_state[offset + 22] = move_data(move_1_id(p)).bp/130.0;
	move_type[move_data(move_1_id(p)).type] = 0;
	move_type[move_data(move_2_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 23);
	game_state[offset + 38] = move_2_pp(p) > 0;
	game_state[offset + 39] = move_data(move_2_id(p)).bp/130.0;
	move_type[move_data(move_2_id(p)).type] = 0;
	move_type[move_data(move_3_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 40);
	game_state[offset + 55] = move_3_pp(p) > 0;
	game_state[offset + 56] = move_data(move_3_id(p)).bp/130.0;
	move_type[move_data(move_3_id(p)).type] = 0;
	move_type[move_data(move_4_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 57);
	game_state[offset + 72] = move_4_pp(p) > 0;
	game_state[offset + 73] = move_data(move_4_id(p)).bp/130.0;

	std::vector<double> st = decode_status((int)status(p));
	std::ranges::copy(st, game_state.begin() + offset + 74);

	move_type[move_data(move_4_id(p)).type] = 0;
	std::array<uint8_t, 2> types = get_type(species(p));
	move_type[types[0]] = 1;
	move_type[types[1]] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 81);
}

inline void extract_imperfect_pokemon_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, pkmn_player player, uint8_t p_num, int offset,std::pair<RevealedSideMask,RevealedSideMask> masks) {
	RangeView<uint8_t*> team = player == PKMN_PLAYER_P1 ? side1(battle_) : side2(battle_);
	RangeView<uint8_t*> p = pokemon(team, p_num);
	game_state[offset] = static_cast<double>(current_hp(p)) / hitpoints(p);
    game_state[offset + 1] = static_cast<double>(hitpoints(p))/481; //Normalization by the maximum value of the stat
	game_state[offset + 2] = static_cast<double>(attack(p)) / 274; 
	game_state[offset + 3] = static_cast<double>(defense(p)) / 335;
	game_state[offset + 4] = static_cast<double>(speed(p)) / 288;
	game_state[offset + 5] = static_cast<double>(special(p)) / 259;
	std::vector<double> move_type(15);
	move_type[move_data(move_1_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 6);
	game_state[offset + 21] = move_1_pp(p) > 0;
	game_state[offset + 22] = move_data(move_1_id(p)).bp/130.0;
	move_type[move_data(move_1_id(p)).type] = 0;
	move_type[move_data(move_2_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 23);
	game_state[offset + 38] = move_2_pp(p) > 0;
	game_state[offset + 39] = move_data(move_2_id(p)).bp/130.0;
	move_type[move_data(move_2_id(p)).type] = 0;
	move_type[move_data(move_3_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 40);
	game_state[offset + 55] = move_3_pp(p) > 0;
	game_state[offset + 56] = move_data(move_3_id(p)).bp/130.0;
	move_type[move_data(move_3_id(p)).type] = 0;
	move_type[move_data(move_4_id(p)).type] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 57);
	game_state[offset + 72] = move_4_pp(p) > 0;
	game_state[offset + 73] = move_data(move_4_id(p)).bp/130.0;

	std::vector<double> st = decode_status((int)status(p));
	std::ranges::copy(st, game_state.begin() + offset + 74);

	move_type[move_data(move_4_id(p)).type] = 0;
	std::array<uint8_t, 2> types = get_type(species(p));
	move_type[types[0]] = 1;
	move_type[types[1]] = 1;
	std::ranges::copy(move_type, game_state.begin() + offset + 81);
}

inline void extract_perfect_active_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, pkmn_player player, int offset) {
    RangeView<uint8_t*> team = player == PKMN_PLAYER_P1 ? side1(battle_) : side2(battle_);
    RangeView<uint8_t*> ord = order(team);
    uint8_t act_pos = ord[0] - 1;
    RangeView<uint8_t*> p = active(team);
    extract_perfect_pokemon_state(battle_,game_state, player, act_pos, offset);
    //binding, invulnerable, confusion, substitute, recharging, leech seed, lightscreen, reflect
    std::vector<bool> vol = { binding(p), invulnerable(p), confusion(p), substitute(p),recharging(p), leechSeed(p), lightScreen(p), reflect(p) };
    std::ranges::copy(vol, game_state.begin() + offset + 96);
    std::vector<float> boosts = { boost_atk(p) / 6.0f, boost_def(p) / 6.0f,boost_spe(p) / 6.0f, boost_spec(p) / 6.0f, boost_acc(p) / 6.0f, boost_eva(p) / 6.0f };
    std::ranges::copy(boosts, game_state.begin() + offset + 100);
}

inline void extract_imperfect_active_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, pkmn_player player, int offset,std::pair<RevealedSideMask,RevealedSideMask> masks) {
    RangeView<uint8_t*> team = player == PKMN_PLAYER_P1 ? side1(battle_) : side2(battle_);
    RangeView<uint8_t*> ord = order(team);
    uint8_t act_pos = ord[0] - 1;
    RangeView<uint8_t*> p = active(team);
    extract_imperfect_pokemon_state(battle_,game_state, player, act_pos, offset, masks);
    //binding, invulnerable, confusion, substitute, recharging, leech seed, lightscreen, reflect
    std::vector<bool> vol = { binding(p), invulnerable(p), confusion(p), substitute(p),recharging(p), leechSeed(p), lightScreen(p), reflect(p) };
    std::ranges::copy(vol, game_state.begin() + offset + 96);
    std::vector<float> boosts = { boost_atk(p) / 6.0f, boost_def(p) / 6.0f,boost_spe(p) / 6.0f, boost_spec(p) / 6.0f, boost_acc(p) / 6.0f, boost_eva(p) / 6.0f };
    std::ranges::copy(boosts, game_state.begin() + offset + 100);
}

inline void extract_perfect_side_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, uint8_t state_side_offset, pkmn_player player) {
    for (uint8_t p_num = 0; p_num < 6; p_num++) {
            extract_perfect_pokemon_state(battle_,game_state, player, p_num, game_state_sizes["pokemon"] * p_num);
    }
    extract_perfect_active_state(battle_,game_state, player, game_state_sizes["inactives"]);
}

inline void extract_imperfect_side_state(pkmn_gen1_battle& battle_, std::vector<float>& game_state, uint8_t state_side_offset, pkmn_player player,std::pair<RevealedSideMask,RevealedSideMask> masks) {
    for (uint8_t p_num = 0; p_num < 6; p_num++) {
            extract_imperfect_pokemon_state(battle_,game_state, player, p_num, game_state_sizes["side"]+game_state_sizes["pokemon"] * p_num, masks);
    }
    extract_imperfect_active_state(battle_,game_state, player, game_state_sizes["side"]+game_state_sizes["inactives"], masks);
}

inline std::vector<float> extract_imperfect_game_state(pkmn_gen1_battle& battle_,std::pair<RevealedSideMask,RevealedSideMask> masks, bool p1) {
    std::vector<float> game_state(1372, 0.0f);
    if (p1) {
        extract_perfect_side_state(battle_,game_state, 0, PKMN_PLAYER_P1);
        extract_imperfect_side_state(battle_,game_state, 1, PKMN_PLAYER_P2, masks);
    }
    else {
        extract_imperfect_side_state(battle_,game_state, 1, PKMN_PLAYER_P1, masks);
        extract_perfect_side_state(battle_,game_state, 0, PKMN_PLAYER_P2);
    }
    assert(game_state.size() == 1372);
    return game_state;
}

std::array<std::tuple<int,int,int,int,int>> stats = {
    {261,151,174,203,167},
    {260,157,180,207,175},
    {271,175,196,221,192},
    {253,158,166,205,178},
    {260,162,174,209,185},
    {268,178,189,221,199},
    {262,151,205,166,178},
    {265,163,212,176,187},
    {273,179,224,191,202},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {251,125,153,184,199},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {271,188,145,201,153},
    {263,151,165,195,156},
    {281,163,177,204,169},
    {282,177,189,214,182},
    {235,164,150,215,132},
    {237,176,164,220,149},
    {252,171,141,212,142},
    {252,190,172,224,166},
    {246,173,167,187,160},
    {254,189,185,202,178},
    {238,158,138,242,172},
    {241,187,155,221,207},
    {267,195,236,157,139},
    {270,207,242,174,159},
    {282,149,182,162,160},
    {283,161,191,173,171},
    {286,175,202,186,184},
    {266,167,160,178,160},
    {268,178,174,187,171},
    {272,190,187,199,184},
    {303,143,171,148,192},
    {293,158,181,162,199},
    {246,136,157,201,201},
    {260,167,184,221,221},
    {386,144,123,123,132},
    {369,162,144,144,151},
    {283,162,168,208,178},
    {277,182,186,217,194},
    {264,155,187,142,223},
    {266,166,196,146,220},
    {270,177,204,151,227},
    {246,191,187,133,187},
    {251,202,199,122,199},
    {285,160,174,166,157},
    {256,150,162,207,207},
    {193,157,127,248,162},
    {201,170,145,248,175},
    {242,138,143,237,152},
    {245,156,160,240,167},
    {270,157,173,185,176},
    {274,178,191,202,194},
    {252,207,150,212,150},
    {255,215,166,220,166},
    {279,189,167,194,176},
    {289,220,194,217,194},
    {244,94,153,239,153},
    {265,110,181,220,157},
    {286,180,214,177,177},
    {214,41,108,234,259},
    {212,59,118,229,251},
    {216,75,129,231,251},
    {306,207,176,150,150},
    {295,221,193,153,161},
    {293,253,197,159,174},
    {267,195,148,157,210},
    {268,202,159,167,215},
    {271,209,170,177,221},
    {244,77,145,205,256},
    {267,156,167,218,248},
    {250,204,262,122,139},
    {252,210,263,135,151},
    {260,208,255,134,149},
    {256,204,175,234,192},
    {252,205,179,232,194},
    {323,117,192,108,150},
    {270,109,217,109,177},
    {223,125,210,166,254},
    {232,147,220,182,258},
    {241,158,163,171,168},
    {238,211,164,216,146},
    {238,214,175,218,160},
    {294,143,183,166,210},
    {286,158,192,177,214},
    {327,209,178,133,160},
    {316,215,189,151,174},
    {237,182,268,160,169},
    {215,184,322,168,189},
    {219,118,131,214,248},
    {219,128,140,214,244},
    {222,138,149,217,245},
    {220,130,335,191,127},
    {272,89,158,153,234},
    {271,113,172,168,237},
    {235,251,247,176,132},
    {240,253,250,189,151},
    {250,116,174,262,183},
    {247,131,182,288,197},
    {272,128,217,150,183},
    {270,179,183,143,238},
    {270,153,256,150,158},
    {257,184,252,149,157},
    {238,244,160,213,132},
    {244,226,205,201,135},
    {300,143,194,124,171},
    {255,182,259,151,196},
    {255,192,258,166,204},
    {306,204,242,125,133},
    {284,227,231,122,129},
    {481,14,75,136,211},
    {249,135,244,162,221},
    {304,192,189,204,131},
    {232,134,210,192,210},
    {243,156,222,207,222},
    {259,181,192,197,174},
    {285,200,178,183,202},
    {222,83,175,225,200},
    {222,109,183,224,204},
    {214,75,172,209,224},
    {259,220,194,232,157},
    {229,118,115,197,197},
    {249,177,158,229,199},
    {255,200,162,217,204},
    {252,242,224,202,157},
    {243,186,197,217,163},
    {0,0,0,0,0},
    {293,239,190,193,221},
    {322,168,179,151,200},
    {299,168,194,194,194},
    {276,160,174,183,201},
    {345,150,162,170,236},
    {232,140,151,248,220},
    {255,253,166,174,242},
    {255,147,182,136,189},
    {238,132,259,146,242},
    {256,143,258,155,244},
    {232,204,245,183,166},
    {244,227,232,194,179},
    {274,212,172,269,164},
    {363,203,158,110,158},
    {271,170,210,189,245},
    {263,172,183,204,238},
    {282,199,204,204,255},
    {254,178,167,176,176},
    {262,193,183,191,191},
    {287,252,214,192,221},
    {253,139,168,216,245},
    {261,175,192,192,192}
}

std::array<float,15> pkmn_type_prior = {0.171233, 0.0547945, 0.116438, 0.205479, 0.0958904, 0.0753425, 0.0547945, 0.0205479, 0.0821918, 0.212329, 0.0890411, 0.0616438, 0.0958904, 0.0342466, 0.0205479};
std::array<float,15> move_type_prior = {0.275918, 0.0386297, 0.0115571, 0.00505744, 0.0357183, 0.0243307, 0.00184172, 0.00752401, 0.0221401, 0.0469237, 0.0611026, 0.0760208, 0.0869736, 0.0624314, 0};
float move_bp_prior = 0,3350625;