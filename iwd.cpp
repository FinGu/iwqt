#include "iwd.hpp"
#include "device.hpp"
#include "agent.hpp"

#include <map>

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

std::optional<device> iwd::get_first_device() {
    managed_objects mobjs = get_objects();
    sdbus::ObjectPath adapter_path;

    for(const auto &[objpath, interfaces] : mobjs) {
        if (interfaces.find(iwd_constants::ADAPTER_IFACE) == interfaces.end()) {
            continue;
        }

        adapter_path = objpath;
    }

    sdbus::ObjectPath device_path;

    for(const auto &[path, _]: mobjs) {
        if(path.find(adapter_path) == 0 && path != adapter_path) {
            device_path = path;
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

    return device(this, name, device_path, powered);
}
