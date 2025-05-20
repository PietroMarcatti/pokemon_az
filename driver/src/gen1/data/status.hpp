#pragma once

#include <cstdint>

namespace pkmndriver {
    namespace Gen1 {
        enum Status : std::uint8_t {
            HLT = 0,
            SLP = 2,
            PSN,
            BRN,
            FRZ,
            PAR,
            EXT,
        };
    }
}