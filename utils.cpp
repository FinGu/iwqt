#include "utils.hpp"

#include <QObject>
#include <QMessageBox>
#include <QCoreApplication>

namespace Utils{
    void adapterNotFound(QWidget *parent) {
        QMessageBox::critical(parent, QCoreApplication::applicationName(), QObject::tr("No adapters available!"));
    }

    void deviceNotFound(QWidget *parent) {
        QMessageBox::warning(parent, QCoreApplication::applicationName(), QObject::tr("Couldn't get an available device"));
    }

    void networkConfigure(const std::string &type, QWidget *parent){
        QString data = QObject::tr("Something is wrong with the %1 network configuration. These types of network need to be configured through the Known Networks menu.")
            .arg(type);

        QMessageBox::warning(parent, QCoreApplication::applicationName(), data);
    }

    QPixmap getIcon(const char *name){
        return QPixmap(name).scaled(TRAY_ICON_SCALE);
    }
}


