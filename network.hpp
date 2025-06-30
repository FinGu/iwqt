#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <cstdint>
#include <sdbus-c++/Types.h>
#include <string>

class network {
  public:
    std::string name;
    std::string type;
    bool connected;
    int16_t signal;
    sdbus::ObjectPath device_path;

    enum strength_type {
        WEAK,
        MODERATE,
        STRONG
    };

    strength_type strength();
};

#endif
