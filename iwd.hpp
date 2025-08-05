#ifndef IWD_HPP
#define IWD_HPP

#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/sdbus-c++.h>
#include <optional>

#include "agent.hpp"

using managed_objects = std::map<
                        sdbus::ObjectPath,
                        std::map<std::string, std::map<std::string, sdbus::Variant>>
                        >;

namespace iwd_constants {
inline constexpr auto AGENT_OBJECT_PATH = "/agent/0";
inline constexpr auto SERVICE_NAME      = "net.connman.iwd";
inline constexpr auto ADAPTER_IFACE     = "net.connman.iwd.Adapter";
inline constexpr auto DEVICE_IFACE      = "net.connman.iwd.Device";
inline constexpr auto STATION_IFACE      = "net.connman.iwd.Station";
inline constexpr auto NETWORK_IFACE      = "net.connman.iwd.Network";
inline constexpr auto AGENTMANAGER_IFACE = "net.connman.iwd.AgentManager";
inline constexpr auto KNOWNNETWORK_IFACE = "net.connman.iwd.KnownNetwork";
}

class adapter;
class known_network;

class iwd {
  public:
    iwd();

    std::optional<adapter> get_first_adapter();

    std::unique_ptr<sdbus::IConnection> system_bus;
    sdbus::ServiceName service_name;

    void register_agent(const agent_ui ui);
    std::unique_ptr<agent> agent_instance;

    void forget_known_network(const known_network&);
    void forget_known_network(const sdbus::ObjectPath&);

    std::vector<known_network> known_networks();

    managed_objects get_objects();
};
#endif
