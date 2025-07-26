#include "iwd.hpp"

#include "device.hpp"

#include "agent.hpp"

#include "tray.hpp"

#include <QApplication>

int main(int argc, char **argv) {
    auto wrapper = iwd();

    QApplication::setApplicationName("IWQt");
    QApplication app(argc, argv);

    Tray tray {wrapper};

    tray.show();

    return app.exec();
}
