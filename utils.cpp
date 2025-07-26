#include "utils.hpp"

#include <QObject>
#include <QMessageBox>
#include <QCoreApplication>

namespace Utils{
    void iwdNotUp(QWidget *parent) {
        QMessageBox::warning(parent, QCoreApplication::applicationName(), QObject::tr("IWD not running"));
    }

    void networkConfigure(const std::string &type, QWidget *parent){
        QString data = QObject::tr("Something is wrong with the %1 network configuration. These types of network need to be configured through the Known Networks menu.")
            .arg(type);

        QMessageBox::warning(parent, QCoreApplication::applicationName(), data);
    }
}


