#include "manage_window.hpp"

#include "network.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <cstdlib>

#define REMOVE_WIDGET(widget) if(widget){ layout->removeWidget(widget); delete widget; widget = NULL; } 

enum networkTypes : int{
    OPEN = 0,
    PSK,
    _8021x,
};

QStringList ManageWindow::supportedTypes(){
    return (QStringList() << "open" << "psk" << "8021x");
}

void ManageWindow::newWindow(){
    networkName = new QLineEdit(this);    
    networkTypes = new QComboBox(this);
    autoconnectEnabled = new QCheckBox(this);
    networkPassword = new QLineEdit(this);

    networkLabel = new QLabel("Network", this);
    typeLabel = new QLabel("Type", this);
    autoconnectLabel = new QLabel("Autoconnect", this);
    passwordLabel = new QLabel("Password", this);
}

void ManageWindow::delWindow(){
    REMOVE_WIDGET(networkLabel);
    REMOVE_WIDGET(typeLabel);
    REMOVE_WIDGET(networkTypes);
    REMOVE_WIDGET(autoconnectLabel);
    REMOVE_WIDGET(autoconnectEnabled);
    REMOVE_WIDGET(passwordLabel);
    REMOVE_WIDGET(networkPassword);
}

void ManageWindow::refreshWindow(bool should_reset){
    if(should_reset){
        delWindow();

        newWindow();
    }

    layout->addWidget(networkLabel);
    layout->addWidget(networkName);

    layout->addWidget(typeLabel);
    layout->addWidget(networkTypes);

    if(networkTypes->currentIndex() == networkTypes::PSK){
        layout->addWidget(passwordLabel);
        layout->addWidget(networkPassword);
    } else{
        REMOVE_WIDGET(passwordLabel);
        REMOVE_WIDGET(networkPassword);
    }

    layout->addWidget(autoconnectLabel);
    layout->addWidget(autoconnectEnabled);
}

ManageWindow::ManageWindow(const known_network *n, QWidget *parent): QDialog(parent){
    layout = new QVBoxLayout(this);

    newWindow();

    if(n){
        fillUpFromNetwork(n);
    }

    refreshWindow(false);

    setLayout(layout);
}

void ManageWindow::fillUpFromNetwork(const known_network *n){
    networkName->setText(n->name.c_str());

    auto selected = 0;
    auto types = supportedTypes();

    for(; selected < types.length(); ++selected){
        if(types[selected] == n->type){
            break;
        }
    }

    networkTypes->addItems(types);

    if(selected >= types.length()){
        selected = 0;
    }

    networkTypes->setCurrentIndex(selected);

    switch(networkTypes->currentIndex()){
        case networkTypes::PSK:
            break;
        case networkTypes::_8021x:
            break;
        default:
            break;
    }
}
