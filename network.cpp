#include "network.hpp"

network::strength_type network::strength() {
    auto sig = signal / 100;

    if (sig >= -60) {
        return network::strength_type::STRONG;
    }

    if (sig >= -80) {
        return network::strength_type::MODERATE;
    }

    return network::strength_type::WEAK;
}
