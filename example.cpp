#include <assert.h>
#include <cstring>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <pkmn.h>


inline pkmn_choice choose(
    pkmn_gen1_battle* battle,
    pkmn_psrng* random,
    pkmn_player player,
    pkmn_choice_kind request,
    pkmn_choice choices[])
{
    uint8_t n = pkmn_gen1_battle_choices(battle, player, request, choices, PKMN_CHOICES_SIZE);
    // Technically due to Generation I's Transform + Mirror Move/Metronome PP
    // error if the battle contains Pokémon with a combination of Transform,
    // Mirror Move/Metronome, and Disable its possible that there are no
    // available choices (softlock), though this is impossible here given that
    // our example battle involves none of these moves
    assert(n > 0);
    // pkmn_gen1_battle_choices determines what the possible choices are - the
    // simplest way to choose an option here is to just use the PSRNG to pick one
    // at random
    return choices[(uint64_t)pkmn_psrng_next(random) * n / 0x100000000];
}

int main(int argc, char** argv)
{

    // We could use C's srand() and rand() function but for point of example
    // we will demonstrate how to use the PSRNG that is exposed by libpkmn
    pkmn_psrng random;
    pkmn_psrng_init(&random, 12345);
    // Preallocate a small buffer for the choice options throughout the battle
    pkmn_choice choices[9];
    auto start = std::chrono::high_resolution_clock::now();
    pkmn_gen1_battle temp_battle = { {
           0x25, 0x01, 0xc4, 0x00, 0xc4, 0x00, 0xbc, 0x00, 0xe4, 0x00, 0x4f, 0x18, 0x0e, 0x30, 0x4b, 0x28,
           0x22, 0x18, 0x25, 0x01, 0x00, 0x01, 0x3a, 0x64, 0x19, 0x01, 0xca, 0x00, 0xb8, 0x00, 0xe4, 0x00,
           0xc6, 0x00, 0x7e, 0x08, 0x53, 0x18, 0xa3, 0x20, 0x44, 0x20, 0x19, 0x01, 0x00, 0x04, 0x88, 0x64,
           0x23, 0x01, 0xc2, 0x00, 0xe4, 0x00, 0xb8, 0x00, 0xc6, 0x00, 0x39, 0x18, 0x3b, 0x08, 0x22, 0x18,
           0x9c, 0x10, 0x23, 0x01, 0x00, 0x07, 0x99, 0x64, 0x11, 0x01, 0xd0, 0x00, 0x9e, 0x00, 0x16, 0x01,
           0xc6, 0x00, 0x55, 0x18, 0x56, 0x20, 0x39, 0x18, 0x45, 0x20, 0x11, 0x01, 0x00, 0x19, 0xbb, 0x64,
           0x07, 0x01, 0xd2, 0x00, 0xa8, 0x00, 0xf2, 0x00, 0x94, 0x00, 0xa2, 0x10, 0x22, 0x18, 0x3b, 0x08,
           0x55, 0x18, 0x07, 0x01, 0x00, 0x13, 0x00, 0x64, 0x1b, 0x01, 0xbc, 0x00, 0xb2, 0x00, 0xd2, 0x00,
           0xa8, 0x00, 0x26, 0x18, 0x62, 0x30, 0x11, 0x38, 0x77, 0x20, 0x1b, 0x01, 0x00, 0x10, 0x20, 0x64,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x61, 0x01, 0x2a, 0x01, 0x20, 0x01, 0x3e, 0x01,
           0xee, 0x00, 0x22, 0x18, 0x3f, 0x08, 0x3b, 0x08, 0x59, 0x10, 0x61, 0x01, 0x00, 0x80, 0x00, 0x64,
           0xbf, 0x02, 0x6c, 0x00, 0x6c, 0x00, 0xc6, 0x00, 0x34, 0x01, 0x73, 0x20, 0x45, 0x20, 0x87, 0x10,
           0x56, 0x20, 0xbf, 0x02, 0x00, 0x71, 0x00, 0x64, 0x0b, 0x02, 0x3e, 0x01, 0xe4, 0x00, 0x9e, 0x00,
           0xe4, 0x00, 0x22, 0x18, 0x73, 0x20, 0x9c, 0x10, 0x3a, 0x10, 0x0b, 0x02, 0x00, 0x8f, 0x00, 0x64,
           0x89, 0x01, 0x20, 0x01, 0x0c, 0x01, 0xd0, 0x00, 0x5c, 0x01, 0x4f, 0x18, 0x5e, 0x10, 0x99, 0x08,
           0x26, 0x18, 0x89, 0x01, 0x00, 0x67, 0xca, 0x64, 0x43, 0x01, 0xf8, 0x00, 0x0c, 0x01, 0x48, 0x01,
           0x2a, 0x01, 0x69, 0x20, 0x56, 0x20, 0x3b, 0x08, 0x55, 0x18, 0x43, 0x01, 0x00, 0x79, 0xc9, 0x64,
           0x39, 0x01, 0xc6, 0x00, 0xbc, 0x00, 0x52, 0x01, 0x70, 0x01, 0x5e, 0x10, 0x45, 0x20, 0x56, 0x20,
           0x69, 0x20, 0x39, 0x01, 0x00, 0x41, 0xcc, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x2e, 0xdb, 0x7d, 0x61, 0xcb, 0xba, 0x0d, 0x1e, 0x7e, 0x9e, 0x00,
        } };
    pkmn_gen1_battle battle;
    pkmn_result result;
    for (int i = 0; i < 1000000; i++) {

        // libpkmn doesn't provide any helpers for initializing the battle structure
        // (the library is intended to be wrapped by something with a higher level
        // API). This setup borrows the serialized state of the setup from the Zig
        // example, though will end up with a different result because it's using a
        // different RNG
        memcpy(&battle, &temp_battle, sizeof(battle));

        // Preallocate a buffer for protocol message logs - PKMN_LOGS_SIZE is
        // guaranteed to be large enough for a single update. This will only be
        // written to if -Dlog is enabled - NULL can be used to turn all of the
        // logging into no-ops
        uint8_t buf[128];
        pkmn_gen1_log_options log_options = { .buf = buf, .len = PKMN_LOGS_SIZE };

        // Initialize the battle options with the log options (in this example
        // the chance and calc features are not demonstrated and thus we simply
        // pass NULL). If no optional features are desired (or if they weren't
        // enabled at compile time) the options struct is unnecessary - we can
        // simply pass NULL as the last argument to the update function
        pkmn_gen1_battle_options options;
        pkmn_gen1_battle_options_set(&options, &log_options, NULL, NULL);

        
        // Pedantically these *should* be pkmn_choice_init(PKMN_CHOICE_PASS, 0), but
        // libpkmn commits to always ensuring the pass choice is 0 so we can simplify
        // things here
        pkmn_choice c1 = 0, c2 = 0;
        // We're also taking advantage of the fact that the PKMN_RESULT_NONE is
        // guaranteed to be 0, so we don't actually need to check against it here
        while (!pkmn_result_type(result = pkmn_gen1_battle_update(&battle, c1, c2, &options))) {
            // If -Dlog is enabled we would now do something with the data in `buf`
            // before coming up with our next set of choices
            c1 = choose(&battle, &random, PKMN_PLAYER_P1, pkmn_result_p1(result), choices);
            c2 = choose(&battle, &random, PKMN_PLAYER_P2, pkmn_result_p2(result), choices);
            // We need to reset our options struct after each update, in this case
            // to reset the log stream before it gets written to again. The middle
            // two arguments (for log and chance, respectively) should almost always
            // be NULL for resets, but the final argument may be used for calc
            // overrides. If you have no battle options then this can be skipped
            pkmn_gen1_battle_options_set(&options, NULL, NULL, NULL);
        }
        // The only error that can occur is if we didn't provide a large enough
        // buffer, but PKMN_MAX_LOGS is guaranteed to be large enough so errors here
        // are impossible. Note however that this is tracking a different kind of
        // error than PKMN_RESULT_ERROR
        assert(!pkmn_error(result));
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
    std::cout << "Battles per second: " << static_cast<double>(1000000) / duration.count() * 1000000<<std::endl;

    // The battle is written in native endianness so we need to do a bit-hack to
    // figure out the system's endianess before we can read the 16-bit turn data
    volatile uint32_t endian = 0x01234567;
    uint16_t turns = (*((uint8_t*)(&endian))) == 0x67
        ? battle.bytes[368] | battle.bytes[369] << 8
        : battle.bytes[368] << 8 | battle.bytes[369];

    // The result is from the perspective of P1
    switch (pkmn_result_type(result)) {
    case PKMN_RESULT_WIN: {
        printf("Battle won by Player A after %d turns\n", turns);
        break;
    }
    case PKMN_RESULT_LOSE: {
        printf("Battle won by Player B after %d turns\n", turns);
        break;
    }
    case PKMN_RESULT_TIE: {
        printf("Battle ended in a tie after %d turns\n", turns);
        break;
    }
    case PKMN_RESULT_ERROR: {
        printf("Battle encountered an error after %d turns\n", turns);
        break;
    }
    default: assert(false);
    }

    return 0;
}