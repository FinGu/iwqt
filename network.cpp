#include "network.hpp"

#include <sdbus-c++/IProxy.h>

network::strength_type network::strength() {
    auto sig = signal / 100;
    
    if(sig >= -50){
        return strength_type::EXCELLENT;
    }

    if(sig >= -60){
        return strength_type::GOOD;
    }

    if(sig >= -70){
        return strength_type::FAIR;
    }

    if(sig >= -80){
        return strength_type::WEAK;
    }

    return strength_type::POOR;
}
