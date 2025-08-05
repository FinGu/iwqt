#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <QObject>

#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/sdbus-c++.h>

#include "network.hpp"

class iwd;

class device {
  public:
    iwd *manager;
    
    device() {};

    device(iwd *manager, std::string name, bool powered, sdbus::ObjectPath path, sdbus::ObjectPath adapter): manager(manager), name(name), powered(powered), path(path), parent_adapter(adapter) {}

    std::string name;
    bool powered;
    sdbus::ObjectPath path, parent_adapter;

    void scan();

    void connect(const network&);
    std::unique_ptr<sdbus::IProxy> connect(const network&, std::function<void(std::optional<sdbus::Error>)>);
    void disconnect();

    std::optional<sdbus::ObjectPath> get_connected_network();

    std::vector<network> get_networks();
};

#endif
