#include <string>
#include <QObject>

enum network_type : int{
    OPEN = 0,
    PSK,
    _8021x,
};

struct base_config{
    std::string ssid;
    bool autoconnect;

    base_config(std::string ssid, bool autoconnect): ssid(ssid), autoconnect(autoconnect){}

    virtual void read() = 0;
    virtual bool save() = 0; 
};

struct open_config: public base_config {
    public:
        open_config(): base_config(std::string{}, true){} //ease of usage

        open_config(std::string ssid): base_config(ssid, true){} //to read

        open_config(std::string ssid, bool autoconnect): base_config(ssid, autoconnect){} //to write

        void read() override;
        bool save() override;
};

struct psk_config: public base_config{
    public:
        psk_config(): base_config(std::string{}, true){}

        psk_config(std::string ssid): base_config(ssid, true){}

        psk_config(std::string ssid, std::string psk, bool autoconnect): base_config(ssid, autoconnect), psk(psk){}

        void read() override;
        bool save() override;

        std::string psk;
};

struct eap_config: public base_config {
    public:
        eap_config(): base_config(std::string{}, true){}

        eap_config(std::string ssid): base_config(ssid, true){}

        eap_config(std::string ssid, bool autoconnect, std::string eap_method, std::string eap_identity, std::string phase2_method, std::string phase2_identity, std::string phase2_password): base_config(ssid, autoconnect), eap_method(eap_method), eap_identity(eap_identity), phase2_method(phase2_method), phase2_identity(phase2_identity), phase2_password(phase2_password){}

        void read() override;
        bool save() override;

        std::string eap_method;
        std::string eap_identity;
        std::string phase2_method;
        std::string phase2_identity;
        std::string phase2_password;
};

constexpr auto iwd_networks_folder = "/var/lib/iwd/";

bool save_as_root(const QString &target, const QByteArray &data);
std::string read_as_root(const QString &target);

bool save_network(const open_config &cfg);
bool save_network(const psk_config &cfg);
bool save_network(const eap_config &cfg);

psk_config parse_psk_network(const std::string &data);
eap_config parse_eap_config(const std::string &data);
