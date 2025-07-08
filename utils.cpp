#include "utils.hpp"

#include <QObject>
#include <QMessageBox>

namespace Utils{
    void iwdNotUp(QWidget *parent) {
        QMessageBox::warning(parent, QObject::tr("IWQt"), QObject::tr("IWD not running"));
    }
}


