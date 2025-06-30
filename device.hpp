#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <QObject>

#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/sdbus-c++.h>

#include "network.hpp"

class iwd;

class device {
  private:
    iwd *manager;

  public:
    device() {};

    device(iwd *manager, std::string name, sdbus::ObjectPath path, bool powered): manager(manager), name(name), path(path), powered(powered) {}

    std::string name;
    sdbus::ObjectPath path;
    bool powered;

    void scan();
    void connect(const network&);
    void disconnect();

    std::vector<network> get_networks();
};

#endif
