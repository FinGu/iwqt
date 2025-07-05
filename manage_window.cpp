#include "manage_window.hpp"

#include "network.hpp"

#include "file.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include <cstdlib>
#include <iostream>

#define REMOVE_WIDGET(widget) if(widget){ layout->removeWidget(widget); delete widget; widget = NULL; } 

QStringList ManageWindow::supportedTypes(){
    return (QStringList() << "open" << "psk" << "8021x");
}

QStringList ManageWindow::supportedEAPMethods(){
    return (QStringList() << "MSCHAPV2" << "PEAP" << "PWD" << "TLS" << "TTLS");
}

QStringList ManageWindow::supportedPhase2Methods(){
    return (QStringList() << "MSCHAPV2" << "PEAP" << "PWD" << "TLS" << "TTLS");
}

void ManageWindow::newWindow(){
    networkName = new QLineEdit(this);    
    networkTypes = new QComboBox(this);

    networkTypes->addItems(supportedTypes());

    autoconnectEnabled = new QCheckBox(this);

    networkLabel = new QLabel("Network", this);
    typeLabel = new QLabel("Type", this);
    autoconnectLabel = new QLabel("Autoconnect", this);
    
    saveButton = new QPushButton("Save", this);
}

void ManageWindow::refreshWindow(){
    layout->addWidget(networkLabel);
    layout->addWidget(networkName);

    layout->addWidget(typeLabel);
    layout->addWidget(networkTypes);

    switch(networkTypes->currentIndex()){
        case network_type::PSK:
            setMaximumHeight(200);
            REMOVE_WIDGET(passwordLabel);
            REMOVE_WIDGET(networkPassword);

            passwordLabel = new QLabel("Password", this);
            networkPassword = new QLineEdit(this);

            layout->addWidget(passwordLabel);
            layout->addWidget(networkPassword);
            break;
        case network_type::_8021x: 
            setMaximumHeight(300);
            REMOVE_WIDGET(passwordLabel);
            REMOVE_WIDGET(networkPassword);

            eapMethodLabel = new QLabel("EAP Method", this);
            eapMethod = new QComboBox(this);
            eapMethod->addItems(supportedEAPMethods());

            layout->addWidget(eapMethodLabel);
            layout->addWidget(eapMethod);

            eapIdentityLabel = new QLabel("EAP Identity", this);
            eapIdentity = new QLineEdit(this);
            layout->addWidget(eapIdentityLabel);
            layout->addWidget(eapIdentity);

            eapPhase2MethodLabel = new QLabel("EAP Phase 2 Method", this);
            eapPhase2Method = new QComboBox(this);
            eapPhase2Method->addItems(supportedPhase2Methods());

            layout->addWidget(eapPhase2MethodLabel);
            layout->addWidget(eapPhase2Method);

            eapPhase2IdentityLabel = new QLabel("EAP Phase 2 Identity", this);
            eapPhase2Identity = new QLineEdit(this);

            layout->addWidget(eapPhase2IdentityLabel);
            layout->addWidget(eapPhase2Identity);

            passwordLabel = new QLabel("EAP Phase 2 Password", this);
            networkPassword = new QLineEdit(this);

            layout->addWidget(passwordLabel);
            layout->addWidget(networkPassword);

            break;
        default:
            setMaximumHeight(100);
            REMOVE_WIDGET(passwordLabel);
            REMOVE_WIDGET(networkPassword);
            REMOVE_WIDGET(eapMethodLabel);
            REMOVE_WIDGET(eapMethod);
            REMOVE_WIDGET(eapIdentityLabel);
            REMOVE_WIDGET(eapIdentity);
            REMOVE_WIDGET(eapPhase2MethodLabel);
            REMOVE_WIDGET(eapPhase2Method);
            REMOVE_WIDGET(eapPhase2IdentityLabel);
            REMOVE_WIDGET(eapPhase2Identity);

            break;
    }
    
    layout->addWidget(autoconnectLabel);
    layout->addWidget(autoconnectEnabled);

    layout->addWidget(saveButton, 0, Qt::AlignRight);
}

ManageWindow::ManageWindow(const known_network *n, QWidget *parent): QDialog(parent){
    setMinimumWidth(300);

    layout = new QVBoxLayout(this);

    newWindow();

    if(n){
        fillUpFromNetwork(n);
    } else{
        refreshWindow();
    }

    connect(networkTypes, &QComboBox::currentIndexChanged, this, [this](int index) {
        refreshWindow();
    });

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        switch(networkTypes->currentIndex()){
            case network_type::OPEN:
                open_config{networkName->text().toStdString(), autoconnectEnabled->isChecked()}.save();

                break;
            case network_type::PSK:
                psk_config{networkName->text().toStdString(), networkPassword->text().toStdString(), autoconnectEnabled->isChecked()}
                .save();
                break;
            case network_type::_8021x:
                eap_config{
                    networkName->text().toStdString(),
                    autoconnectEnabled->isChecked(),
                    supportedEAPMethods()[eapMethod->currentIndex()].toStdString(),
                    eapIdentity->text().toStdString(),
                    supportedPhase2Methods()[eapPhase2Method->currentIndex()].toStdString(),
                    eapPhase2Identity->text().toStdString(),
                    networkPassword->text().toStdString()
                }.save();
                break;
        }

        this->close();
    });

    setLayout(layout);
}

void ManageWindow::fillUpFromNetwork(const known_network *n){
    networkName->setText(n->name.c_str());

    auto selected = 0;
    auto types = supportedTypes();

    if((selected = types.indexOf(n->type)) < 0){
        selected = 0;
    }

    networkTypes->setCurrentIndex(selected);

    refreshWindow();

    open_config cfg0;
    psk_config cfg1;
    eap_config cfg2;

    switch(selected){
        case network_type::OPEN:
            cfg0 = open_config{n->name};
            cfg0.read();

            autoconnectEnabled->setChecked(cfg0.autoconnect);
            break;

        case network_type::PSK:
            cfg1 = psk_config{n->name};
            cfg1.read();

            autoconnectEnabled->setChecked(cfg1.autoconnect);
            networkPassword->setText(cfg1.psk.c_str());
            break;

        case network_type::_8021x:
            cfg2 = eap_config{n->name};    
            cfg2.read();

            autoconnectEnabled->setChecked(cfg2.autoconnect);

            auto methods = supportedEAPMethods();

            if((selected = methods.indexOf(cfg2.eap_method)) < 0){
                std::cout << "[Warning] Unsupported EAP method\n";
                selected = 0;
            }

            eapMethod->setCurrentIndex(selected);
            eapIdentity->setText(cfg2.eap_identity.c_str());

            auto phase2_methods = supportedPhase2Methods(); 
            if((selected = phase2_methods.indexOf(cfg2.phase2_method)) < 0){
                std::cout << "[Warning] Unsupported EAP Phase2 method\n";
                selected = 0;
            }

            eapPhase2Method->setCurrentIndex(selected);
            eapPhase2Identity->setText(cfg2.phase2_identity.c_str());
            networkPassword->setText(cfg2.phase2_password.c_str());

            break;
    }
}
