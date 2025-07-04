#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <cstdint>
#include <sdbus-c++/Types.h>
#include <string>

struct base_network{
    public:
      std::string name;
      std::string type;
      sdbus::ObjectPath path;
};

class network: public base_network {
  public:
    bool connected;

    int16_t signal;

    enum strength_type {
        WEAK,
        MODERATE,
        STRONG
    };

    strength_type strength();
};

class known_network: public base_network {
  public:
    bool hidden;
    bool autoconnect;
    std::string last_connected;
};

#endif
