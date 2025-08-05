#ifndef ADAPTER_HPP
#define ADAPTER_HPP

#include <QObject>

#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/sdbus-c++.h>

#include "device.hpp"

class iwd;

class adapter {
  private:
    iwd *manager;

  public:
    adapter(){};

    adapter(device &from_device): manager(from_device.manager), path(from_device.parent_adapter){}

    adapter(iwd *manager, sdbus::ObjectPath path): manager(manager), path(path) {};

    sdbus::ObjectPath path;

    void set_powered(bool);

    bool get_powered();

    std::optional<device> get_first_device();
};

#endif
