#include "device.hpp"

#include "iwd.hpp"

#include "network.hpp"

#include <iostream>
#include <optional>
#include <sdbus-c++/Types.h>

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

std::unique_ptr<sdbus::IProxy> device::connect(const network &in, std::function<void(std::optional<sdbus::Error>)> f) {
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    in.path
                                   );

    proxy->callMethodAsync(sdbus::MethodName{"Connect"})
    .onInterface(sdbus::InterfaceName{iwd_constants::NETWORK_IFACE})
    .uponReplyInvoke(f);
    // we can't have this blocking or else we won't be able to show a dialog

    return proxy;
}


void device::connect(const network &in) {
    auto callback = [](std::optional<sdbus::Error> e) {};

    this->connect(in, callback);
}

void device::disconnect() {
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );

    auto call = proxy->createMethodCall(sdbus::InterfaceName{iwd_constants::STATION_IFACE}, sdbus::MethodName{"Disconnect"});

    proxy->callMethod(call);
}

std::optional<sdbus::ObjectPath> device::get_connected_network(){
    auto proxy = sdbus::createProxy(*this->manager->system_bus,
                                    this->manager->service_name,
                                    this->path
                                   );
    try{
        auto prop = proxy->getProperty("ConnectedNetwork").onInterface(iwd_constants::STATION_IFACE);
        return prop.get<sdbus::ObjectPath>();
    } catch(...){
        return std::nullopt;
    }
}

