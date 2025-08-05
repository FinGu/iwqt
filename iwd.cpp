#include "iwd.hpp"
#include "adapter.hpp"
#include "device.hpp"
#include "agent.hpp"

#include <map>
#include <optional>

iwd::iwd() {
    this->system_bus = sdbus::createSystemBusConnection();
    this->service_name = sdbus::ServiceName{iwd_constants::SERVICE_NAME};
}

void iwd::register_agent(const agent_ui ui) {
    this->agent_instance = std::make_unique<agent>(*this->system_bus,
                           sdbus::ObjectPath{iwd_constants::AGENT_OBJECT_PATH},
                           ui
                                                  );

    this->agent_instance->_register(this);

    this->system_bus->enterEventLoopAsync();
}

managed_objects iwd::get_objects() {
    auto proxy = sdbus::createProxy(*system_bus,
                                    service_name,
                                    sdbus::ObjectPath{"/"}
                                   );

    managed_objects mobjs;

    auto call = proxy->createMethodCall(sdbus::InterfaceName{"org.freedesktop.DBus.ObjectManager"}, sdbus::MethodName{"GetManagedObjects"});

    auto reply = proxy->callMethod(call);

    reply >> mobjs;

    return mobjs;
}

std::optional<adapter> iwd::get_first_adapter(){
    managed_objects mobjs = get_objects();
    sdbus::ObjectPath adapter_path;

    for(const auto &[objpath, interfaces] : mobjs) {
        if (interfaces.find(iwd_constants::ADAPTER_IFACE) == interfaces.end()) {
            continue;
        }

        adapter_path = objpath;
    }

    if(adapter_path.empty()){
        return std::nullopt;
    }

    return adapter(this, adapter_path);
}

void iwd::forget_known_network(const sdbus::ObjectPath &path){
    auto proxy = sdbus::createProxy(*system_bus,
                                    service_name,
                                    path
                                   );

    auto call = proxy->createMethodCall(sdbus::InterfaceName{iwd_constants::KNOWNNETWORK_IFACE}, sdbus::MethodName{"Forget"});

    proxy->callMethod(call);
}


void iwd::forget_known_network(const known_network &n){
    return this->forget_known_network(n.path);
}

std::vector<known_network> iwd::known_networks(){
    std::vector<known_network> out;
    managed_objects mobjs = get_objects();

    for(const auto &[objpath, interfaces] : mobjs) {
        if (interfaces.find(iwd_constants::KNOWNNETWORK_IFACE) == interfaces.end()) {
            continue;

        }

        const auto &data = interfaces.at(iwd_constants::KNOWNNETWORK_IFACE);

        out.push_back(known_network{
            {data.at("Name").get<std::string>(), data.at("Type").get<std::string>(), objpath},
            data.at("Hidden").get<bool>(),
            data.at("AutoConnect").get<bool>(),
            data.at("LastConnectedTime").get<std::string>()
        });
    }

    return out;
}
