#include "tray.hpp"

#include "agent.hpp"
#include "iwd.hpp"

#include "device.hpp"

#include "utils.hpp"

#include <QWidgetAction>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>

#include <QWidget>
#include <QInputDialog>
#include <QThread>
#include <QToolTip>

#include <cstdio>
#include <unistd.h>

Tray::Tray(iwd &in): manager(in) {
    createTray();

    createItems();
    instantiateDevice();

    fillMenu();
    createManageWindow();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &Tray::iconActivated);

    refreshTray(true);

    makeAgent();
}

void Tray::createTray() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(FAILURE_ICON_PATH));
}

void Tray::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if(mwindow->isVisible()){
            mwindow->hide();
        } else{
            mwindow->show();
        }
        break;
    default:
        break;
    }
}

void Tray::instantiateDevice() {
    try{
        this->cur_adapter = manager.get_first_adapter().value();
    } catch(...){
        Utils::adapterNotFound(this);
        QCoreApplication::exit(0);
    }

    updateEnabledCheckbox(cur_adapter.get_powered());

    try {
        this->cur_device = this->cur_adapter.get_first_device().value();
    } catch(...) {
        trayIcon->show();

        Utils::deviceNotFound(this);

        for(;;) {
            try {
                this->cur_device = this->cur_adapter.get_first_device().value();
                break;
            } catch(...) {
                QThread::sleep(2);
            }
        }
    }
}

std::string Tray::requestPassphrase(const std::string& path) {
    std::string out;
    bool ok = false;

    QMetaObject::invokeMethod(this, [this, &path, &out, &ok]() {
        QString title = tr("Credentials for:\n%1")
                        .arg(QString::fromStdString(path));

        QString pw = QInputDialog::getText(
                         this,
                         tr("Wi‑Fi Passphrase"),
                         title,
                         QLineEdit::Password,
                         QString(),
                         &ok
                     );
        if(!ok) {
            return;
        }
        out = pw.toStdString();
    },
    Qt::BlockingQueuedConnection
                             );

    return out;
}

std::tuple<std::string,std::string> Tray::requestUserAndPassphrase(const std::string& path) {
    std::string user, pass;
    bool ok;

    QMetaObject::invokeMethod(
        this,
    [&]() {
        QString title = tr("Credentials for:\n%1")
                        .arg(QString::fromStdString(path));
        QString u = QInputDialog::getText(
                        this,
                        title,
                        tr("Username:"),
                        QLineEdit::Normal,
                        QString(),
                        &ok
                    );
        if(!ok) {
            return;
        }

        user = u.toStdString();
    },
    Qt::BlockingQueuedConnection
    );

    if(!ok || user.empty()) {
        return { "canceled", "canceled" };
    }

    QMetaObject::invokeMethod(
        this,
    [&]() {
        QString title = tr("Credentials for:\n%1")
                        .arg(QString::fromStdString(path));
        QString p = QInputDialog::getText(
                        this,
                        title,
                        tr("Password for %1:").arg(QString::fromStdString(user)),
                        QLineEdit::Password,
                        QString(),
                        &ok
                    );
        if(!ok) {
            return;
        }

        pass = p.toStdString();
    },
    Qt::BlockingQueuedConnection
    );

    if(!ok || pass.empty()) {
        return { "canceled", "canceled" };
    }

    return { user, pass };
}

void Tray::makeAgent() {
    agent_ui ui;

    ui.request_password = [this](const std::string& path) -> std::string {
        return this->requestPassphrase(path);
    };

    ui.request_user_and_password = [this](const std::string& path) -> std::tuple<std::string,std::string> {
        return this->requestUserAndPassphrase(path);
    };

    this->manager.register_agent(std::move(ui));
}

void Tray::connectedHandler(network n, QPixmap icon){
    auto connected = this->cur_device.get_connected_network().has_value();

    if(connected){
        QMetaObject::invokeMethod(this, [this, n, icon](){
            trayIcon->setIcon(icon);

            trayIcon->showMessage(
                tr("Connected to %1").arg(n.name),
                tr("Type: %1").arg(n.type),
                QSystemTrayIcon::Information,
                3000
            );

        }, Qt::QueuedConnection);
        return;
    }

    if(n.type == "8021x"){
        QMetaObject::invokeMethod(this, [=]() {
            Utils::networkConfigure(n.type, this);
        });
    }
}

QPixmap Tray::getIconForStrength(network::strength_type st){
    QPixmap icon;
    switch (st) {
        case network::strength_type::EXCELLENT:
            icon = QPixmap(EXCELLENT_ICON_PATH).scaled(TRAY_ICON_SCALE);
            break;
        case network::strength_type::GOOD:
            icon = QPixmap(GOOD_ICON_PATH).scaled(TRAY_ICON_SCALE);
            break;
        case network::strength_type::FAIR:
            icon = QPixmap(FAIR_ICON_PATH).scaled(TRAY_ICON_SCALE);
            break;
        case network::strength_type::WEAK:
            icon = QPixmap(WEAK_ICON_PATH).scaled(TRAY_ICON_SCALE);
            break;
        case network::strength_type::POOR:
            icon = QPixmap(POOR_ICON_PATH).scaled(TRAY_ICON_SCALE);
            break;
    }
    return icon;
}

QPixmap Tray::addNetwork(network n) {
    auto action = networksMenu->addAction(n.name.c_str());

    QPixmap icon = getIconForStrength(n.strength());

    action->setIcon(icon);

    if (n.connected) {
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, &QAction::triggered, this, [=] {
            action->setChecked(true);
        });
        trayIconMenu->setIcon(icon);
        return icon;
    }

    connect(action, &QAction::triggered, this, [=] {
        saved_proxy = this->cur_device.connect(n, [=](std::optional<sdbus::Error> e){
            connectedHandler(n, icon); 
        });
        //needs to be saved so the callback is invoked later on
    });

    return icon;
}

QPixmap Tray::processConnectedNetwork(network n) {
    auto icon = addNetwork(n);

    QAction* disconnectAction = new QAction(tr("&Disconnect"), this);

    connect(disconnectAction, &QAction::triggered, this, [this, n] {
        this->cur_device.disconnect();

        trayIcon->setIcon(QPixmap(DISCONNECTED_ICON_PATH).scaled(TRAY_ICON_SCALE));

        trayIcon->showMessage(
            tr("Disconnected from %1").arg(n.name),
            "",
            QSystemTrayIcon::Information,
            3000
        );
    });

    networksMenu->addAction(disconnectAction);

    QAction* availableLabel = new QAction(tr("&Available"), this);
    availableLabel->setEnabled(false);
    networksMenu->addAction(availableLabel);
    return icon;
}

void Tray::updateEnabledCheckbox(bool powered){
    enabledAdapterAction->setChecked(powered);

    networksMenu->setEnabled(powered);
    scanAction->setEnabled(powered);
}

void Tray::refreshTray(bool should_scan) {
    networksMenu->clear();

    updateEnabledCheckbox(cur_adapter.get_powered());

    if(should_scan) {
        this->cur_device.scan();
    }

    auto inetworks = this->cur_device.get_networks();

    std::sort(inetworks.begin(), inetworks.end(), [](network &a, network &b){
        return a.signal > b.signal;
    });

    auto size = inetworks.size();

    for(size_t i = 0; i < size; ++i) {
        auto network = inetworks[i];

        if(!network.connected) {
            continue;
        }

        trayIcon->setIcon(processConnectedNetwork(network));

        inetworks.erase(inetworks.begin() + i);

        break;
    }

    if(size == inetworks.size()){
        trayIcon->setIcon(QIcon(DISCONNECTED_ICON_PATH));
    }

    for(auto n: inetworks) {
        addNetwork(n);
    }
}

void Tray::setVisible(bool visible) {
    QDialog::setVisible(false);
}

void Tray::createItems() {
    enabledAdapterAction = new QAction(tr("&Enabled"), this);
    enabledAdapterAction->setCheckable(true);

    connect(enabledAdapterAction, &QAction::triggered, this, [this]{
        auto checked = enabledAdapterAction->isChecked();
        cur_adapter.set_powered(checked);
        updateEnabledCheckbox(checked);
    });

    networksMenu = new QMenu(tr("&Networks"), this);
    connect(networksMenu, &QMenu::aboutToShow, this, [this] {
        try {
            this->refreshTray(!settings.value(AVOID_SCANS_SETTING, false).toBool());
        } catch(...) {
            instantiateDevice(); //try to recover
        }
    });

    manageAction = new QAction(tr("&Manage"), this);
    connect(manageAction, &QAction::triggered, this, [this]() {
        mwindow->show();        
    });

    scanAction = new QAction(tr("&Scan"), this);
    connect(scanAction, &QAction::triggered, this, [this]() {
        try {
            this->refreshTray(true);
        } catch(...) {
            instantiateDevice();
        }
    });


    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);
}

void Tray::fillMenu() {
    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction(enabledAdapterAction);

    trayIconMenu->addMenu(networksMenu);
    trayIconMenu->addAction(scanAction);

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(manageAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void Tray::createManageWindow(){
    mwindow = new ManageWindow(manager, this);
}
