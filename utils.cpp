#include "utils.hpp"

#include <QObject>
#include <QMessageBox>
#include <QCoreApplication>

#include <QGuiApplication>
#include <QStyleHints>

#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/sdbus-c++.h>

namespace Utils{
    void adapterNotFound(QWidget *parent) {
        QMessageBox::critical(parent, QCoreApplication::applicationName(), QObject::tr("No adapters available!"));
    }

    void deviceNotFound(QWidget *parent) {
        QMessageBox::warning(parent, QCoreApplication::applicationName(), QObject::tr("Couldn't get an available device"));
    }

    void networkConfigure(const std::string &type, QWidget *parent){
        QString data = QObject::tr("Something is wrong with the %1 network configuration. These types of network need to be configured through the Manage Networks menu.")
            .arg(type);

        QMessageBox::warning(parent, QCoreApplication::applicationName(), data);
    }

    QPixmap getIcon(const char *name){
        return QPixmap(name).scaled(TRAY_ICON_SCALE);
    }

    bool isSystemDarkMode(){
        auto scheme = QGuiApplication::styleHints()->colorScheme();
        if (scheme == Qt::ColorScheme::Dark) {
            return true;
        }

        try {
            auto con = sdbus::createSessionBusConnection();
            auto proxy = sdbus::createProxy(*con,
                sdbus::ServiceName{"org.freedesktop.portal.Desktop"},
                sdbus::ObjectPath{"/org/freedesktop/portal/desktop"});
            auto call = proxy->createMethodCall(
                sdbus::InterfaceName{"org.freedesktop.portal.Settings"},
                sdbus::MethodName{"Read"});
            call << "org.freedesktop.appearance" << "color-scheme";
            auto reply = proxy->callMethod(call);
            sdbus::Variant outer;
            reply >> outer;
            sdbus::Variant inner = outer.get<sdbus::Variant>();
            return inner.get<uint32_t>() == 2;
        } catch (...) {
        }

        if (scheme == Qt::ColorScheme::Unknown) {
            auto pal = QGuiApplication::palette();
            return pal.window().color().value() < pal.windowText().color().value();
        }

        return false;
    }
}
