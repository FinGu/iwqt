#include "window.hpp"

#include "agent.hpp"
#include "iwd.hpp"

#include "device.hpp"

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

#include <cstdio>
#include <unistd.h>

Window::Window(iwd &in): manager(in) {
    createTray();

    createItems();
    instantiateDevice();

    fillMenu();

    this->cur_device.scan();

    makeAgent();
}

void Window::iwdNotUp() {
    QMessageBox::warning(this, tr("IWQt"), tr("IWD not running"));
}

void Window::createTray() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/good.png"));
}

void Window::instantiateDevice() {
    try {
        this->cur_device = manager.get_first_device().value();
    } catch(...) {
        trayIcon->show();

        iwdNotUp();

        for(;;) {
            try {
                this->cur_device = manager.get_first_device().value();
                break;
            } catch(...) {
                QThread::sleep(2);
            }
        }
    }
}

std::string Window::requestPassphrase(const std::string& path) {
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

std::tuple<std::string,std::string> Window::requestUserAndPassphrase(const std::string& path) {
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



void Window::makeAgent() {
    agent_ui ui;

    ui.request_password = [this](const std::string& path) -> std::string {
        return this->requestPassphrase(path);
    };

    ui.request_user_and_password = [this](const std::string& path) -> std::tuple<std::string,std::string> {
        return this->requestUserAndPassphrase(path);
    };

    this->manager.register_agent(std::move(ui));
}

void Window::addNetwork(network n) {
    QIcon icon;
    auto action = networksMenu->addAction(n.name.c_str());

    switch(n.strength()) {
    case network::strength_type::WEAK:
        icon = QIcon(":/images/bad.png");
        break;
    case network::strength_type::MODERATE:
        icon = QIcon(":/images/mid.png");
        break;
    case network::strength_type::STRONG:
        icon = QIcon(":/images/good.png");
        break;
    }

    action->setIcon(icon);

    if (n.connected) {
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, &QAction::triggered, this, [=] {
            action->setChecked(true);
        });
        trayIconMenu->setIcon(icon);
        return;
    }

    connect(action, &QAction::triggered, this, [=] {
        this->cur_device.connect(n);
    });
}

void Window::processConnectedNetwork(network n) {
    addNetwork(n);

    QAction* disconnectAction = new QAction(tr("&Disconnect"), this);

    connect(disconnectAction, &QAction::triggered, this, [this] {
        this->cur_device.disconnect();
    });

    networksMenu->addAction(disconnectAction);

    QAction* availableLabel = new QAction(tr("&Available"), this);
    availableLabel->setEnabled(false);
    networksMenu->addAction(availableLabel);
}

void Window::refreshNetworks(bool should_scan) {
    networksMenu->clear();

    if(should_scan) {
        this->cur_device.scan();
    }

    auto inetworks = this->cur_device.get_networks();

    for(size_t i = 0; i < inetworks.size(); ++i) {
        auto network = inetworks[i];

        if(!network.connected) {
            continue;
        }

        processConnectedNetwork(network);

        inetworks.erase(inetworks.begin() + i);

        break;
    }

    for(auto n: inetworks) {
        addNetwork(n);
    }
}

void Window::setVisible(bool visible) {
    QDialog::setVisible(false);
}

void Window::createItems() {
    avoidScans = new QAction(tr("&Avoid scans"), this);
    avoidScans->setCheckable(true);

    networksMenu = new QMenu(tr("&Networks"), this);
    connect(networksMenu, &QMenu::aboutToShow, this, [this] {
        try {
            this->refreshNetworks(!avoidScans->isChecked());
        } catch(...) {
            iwdNotUp();
        }
    });

    scanAction = new QAction(tr("&Scan"), this);
    connect(scanAction, &QAction::triggered, this, [this]() {
        try {
            this->refreshNetworks(true);
        } catch(...) {
            iwdNotUp();
        }
    });

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);
}

void Window::fillMenu() {
    trayIconMenu = new QMenu(this);

    trayIconMenu->addMenu(networksMenu);
    trayIconMenu->addAction(avoidScans);

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(scanAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

