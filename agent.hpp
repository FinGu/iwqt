#ifndef AGENT_HPP
#define AGENT_HPP

#include "iwd-agent-server-glue.h"

#include <sdbus-c++/IObject.h>
#include <sdbus-c++/Types.h>

class iwd;

struct agent_ui {
    std::function<std::string(const std::string& path)> request_password;

    std::function<std::string(const std::string& path)> request_private_key_password;

    std::function<std::tuple<std::string,std::string>(const std::string& path)> request_user_and_password;
};

class agent : public sdbus::AdaptorInterfaces<net::connman::iwd::Agent_adaptor> {
  private:
    const sdbus::ObjectPath path;
    const agent_ui handler;

  public:
    agent(sdbus::IConnection& con, sdbus::ObjectPath objectPath, agent_ui handler);
    ~agent();

    void _register(iwd *manager);

    void Release() override;
    std::string RequestPassphrase(const sdbus::ObjectPath& path) override;
    std::string RequestPrivateKeyPassphrase(const sdbus::ObjectPath& path) override;
    std::tuple<std::string, std::string> RequestUserNameAndPassword(const sdbus::ObjectPath& path) override;
    void Cancel(const std::string& reason) override;

};

#endif
