#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/Types.h>
#include <sys/wait.h>

#include "iwd.hpp"

#include "device.hpp"

#include "agent.hpp"

#include "window.hpp"

#include <QApplication>

int main(int argc, char **argv) {
    auto wrapper = iwd();

    QApplication app(argc, argv);

    Window window {wrapper};

    window.show();

    return app.exec();
}
