#include "file.hpp"

#include <QProcess>

#include <sstream>
#include <cstdlib>
#include <string>

#define GET_VALUE(param, key_text)                                  \
    do {                                                            \
        const std::string key = std::string(key_text) + "=";        \
        auto pos = data.find(key);                                  \
        if (pos == std::string::npos) break;                        \
        pos += key.size();                                          \
        std::istringstream iss(data.substr(pos));                   \
        std::getline(iss, param);                                   \
    } while (0)

constexpr auto root_exec = "pkexec";

bool save_as_root(const QString &target, const QByteArray &data) {
    QStringList args;
    args << "sh" << "-c" << QString("cat > %1").arg(target);

    QProcess proc;
    proc.start(root_exec, args);

    if (!proc.waitForStarted()) {
        return false;
    }

    proc.write(data);
    proc.closeWriteChannel();

    if (!proc.waitForFinished()) {
        return false;
    }

    if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        return false;
    }

    return true;
}

std::string read_as_root(const QString &target) {
    auto nil = std::string{}; 

    QStringList args;
    args << "sh" << "-c" << QString("cat %1").arg(target);

    QProcess proc;
    proc.start(root_exec, args);

    if (!proc.waitForStarted()){
        return nil;
    }

    if (!proc.waitForFinished()){
        return nil;
    }

    if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0){
        return nil;
    }

    return proc.readAllStandardOutput().toStdString();
}

bool open_config::save(){
    QByteArray data = QString("[Settings]\nAutoConnect=%1\n")
        .arg(this->autoconnect ? "True" : "False")
        .toUtf8();

    auto path = QString("%1%2.%3").arg(iwd_networks_folder).arg(this->ssid).arg("open");

    return save_as_root(path, data);
}

void open_config::read(){
    auto path = QString("%1%2.%3").arg(iwd_networks_folder)
            .arg(this->ssid)
            .arg("open");

    std::string autoconnect;

    auto data = read_as_root(path);

    GET_VALUE(autoconnect, "AutoConnect");

    if(autoconnect.empty()){
        this->autoconnect = true;
    } else{
        this->autoconnect = autoconnect == "True";
    } 
}

bool psk_config::save(){
    QByteArray data = QString("[Security]\nPassphrase=%1\n[Settings]\nAutoConnect=%2\n")
        .arg(this->psk)
        .arg(this->autoconnect ? "True" : "False")
        .toUtf8();

    auto path = QString("%1%2.%3").arg(iwd_networks_folder).arg(this->ssid).arg("psk");

    return save_as_root(path, data);
} 

bool eap_config::save(){
    auto prefix = this->eap_method;

    QByteArray data = QString("[Security]\nEAP-Method=%1\nEAP-Identity=%2\nEAP-%1-Phase2-Method=%3\nEAP-%1-Phase2-Identity=%4\nEAP-%1-Phase2-Password=%5\n[Settings]\nAutoConnect=%6\n")
        .arg(prefix)
        .arg(this->eap_identity)
        .arg(this->phase2_method)
        .arg(this->phase2_identity)
        .arg(this->phase2_password)
        .arg(this->autoconnect ? "True" : "False")
        .toUtf8();

    auto path = QString("%1%2.%3").arg(iwd_networks_folder).arg(this->ssid).arg("8021x");

    return save_as_root(path, data);
} 

void psk_config::read(){
    auto path = QString("%1%2.%3").arg(iwd_networks_folder)
            .arg(this->ssid)
            .arg("psk");

    auto data = read_as_root(path);

    std::string autoconnect;
    GET_VALUE(autoconnect, "AutoConnect");

    if(autoconnect.empty()){
        this->autoconnect = true;
    } else{
        this->autoconnect = autoconnect == "True";
    }    

    GET_VALUE(this->psk, "Passphrase");
}

void eap_config::read(){
    auto path = QString("%1%2.%3").arg(iwd_networks_folder)
            .arg(this->ssid)
            .arg("8021x");

    auto data = read_as_root(path);

    std::string autoconnect;
    GET_VALUE(autoconnect, "AutoConnect");

    if(autoconnect.empty()){
        this->autoconnect = true;
    } else{
        this->autoconnect = autoconnect == "True";
    } 

    GET_VALUE(this->eap_method, "EAP-Method");

    GET_VALUE(this->eap_identity, "EAP-Identity");

    if(this->eap_method.empty()){
        return;
    }

    std::string prefix = "EAP-";

    GET_VALUE(this->phase2_method, prefix + this->eap_method + "-Phase2-Method");
    GET_VALUE(this->phase2_identity, prefix + this->eap_method + "-Phase2-Identity");
    GET_VALUE(this->phase2_password, prefix + this->eap_method + "-Phase2-Password");
}
