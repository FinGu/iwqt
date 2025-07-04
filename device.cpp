#include "device.hpp"

#include "iwd.hpp"

#include "network.hpp"

#include <iostream>

void device::scan() {
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );

    try {
        auto call = proxy->createMethodCall(sdbus::InterfaceName{iwd_constants::STATION_IFACE}, sdbus::MethodName{"Scan"});

        proxy->callMethod(call);
    } catch(...) {
        std::cout << "[Warning] Scan in progress\n";
    }
}

std::vector<network> device::get_networks() {
    std::vector<network> out;

    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );

    auto call = proxy->createMethodCall(sdbus::InterfaceName{iwd_constants::STATION_IFACE}, sdbus::MethodName{"GetOrderedNetworks"});

    std::vector<sdbus::Struct<sdbus::ObjectPath, int16_t>> nets;

    auto reply = proxy->callMethod(call);

    reply >> nets;

    managed_objects all_objects = this->manager->get_objects();

    for(const auto &entry: nets) {
        auto path = entry.get<0>();
        auto signal = entry.get<1>();
        auto data = all_objects[path][iwd_constants::NETWORK_IFACE];

        out.push_back(network{
            {data.at("Name").get<std::string>(), data.at("Type").get<std::string>(), path},
            data.at("Connected").get<bool>(),
            signal
        });
    }

    return out;
}

void device::connect(const network &in) {
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    in.path
                                   );

    auto callback = [](std::optional<sdbus::Error> e) {};

    proxy->callMethodAsync(sdbus::MethodName{"Connect"})
    .onInterface(sdbus::InterfaceName{iwd_constants::NETWORK_IFACE})
    .uponReplyInvoke(callback);
    // we can't have this blocking or else we won't be able to show a dialog
}

void device::disconnect() {
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );

    auto call = proxy->createMethodCall(sdbus::InterfaceName{iwd_constants::STATION_IFACE}, sdbus::MethodName{"Disconnect"});

    proxy->callMethod(call);
}
