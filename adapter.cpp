#include "adapter.hpp"

#include "iwd.hpp"

#include <sdbus-c++/Types.h>

void adapter::set_powered(bool enabled){
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );

    proxy->setProperty("Powered")
        .onInterface(iwd_constants::ADAPTER_IFACE)
        .toValue(sdbus::Variant(enabled));
}

bool adapter::get_powered(){
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );

    return proxy->getProperty("Powered")
        .onInterface(iwd_constants::ADAPTER_IFACE)
        .get<bool>();
}

std::optional<device> adapter::get_first_device() {
    managed_objects mobjs = manager->get_objects();

    sdbus::ObjectPath device_path;

    for(const auto &[cur_p, _]: mobjs) {
        if(cur_p.find(this->path) == 0 && cur_p != this->path) {
            device_path = cur_p;
            break;
        }
    }

    const auto& interfaces = mobjs.at(device_path);

    if(interfaces.find(iwd_constants::DEVICE_IFACE) == interfaces.end()) {
        return std::nullopt;
    }

    const auto interface = interfaces.at(iwd_constants::DEVICE_IFACE);

    const auto name = interface.at("Name").get<std::string>();

    const auto powered = interface.at("Powered").get<bool>();

    return device(this->manager, name, powered, device_path, path);
}

