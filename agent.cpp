#include "agent.hpp"

#include "iwd.hpp"

#include <cstdlib>
#include <sdbus-c++/AdaptorInterfaces.h>
#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/VTableItems.h>
#include <sdbus-c++/sdbus-c++.h>

#include <QString>
#include <QInputDialog>

agent::agent(sdbus::IConnection& con, sdbus::ObjectPath obj_path, agent_ui handler): AdaptorInterfaces(con, obj_path), path(obj_path), handler(handler) {
    registerAdaptor();
}

agent::~agent() {
    unregisterAdaptor();
}

void agent::_register(iwd *manager) {
    auto proxy = sdbus::createProxy(*manager->system_bus,
                                    manager->service_name,
                                    sdbus::ObjectPath{"/net/connman/iwd"});

    auto call = proxy->createMethodCall(sdbus::InterfaceName{iwd_constants::AGENTMANAGER_IFACE}, sdbus::MethodName{"RegisterAgent"});

    call << path;

    proxy->callMethod(call);
}

void agent::Release() { /* .. */ }

void agent::Cancel(const std::string& reason) {/* .. */}

std::string agent::RequestPassphrase(const sdbus::ObjectPath& path) {
    return this->handler.request_password(std::string{path});
}

std::string agent::RequestPrivateKeyPassphrase(const sdbus::ObjectPath& path) {
    return this->handler.request_private_key_password(std::string{path});
}

std::tuple<std::string, std::string> agent::RequestUserNameAndPassword(const sdbus::ObjectPath& path) {
    return this->handler.request_user_and_password(std::string{path});
}
