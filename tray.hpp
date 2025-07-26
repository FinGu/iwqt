#ifndef TRAY_HPP
#define TRAY_HPP

#include "iwd.hpp"

#include "device.hpp"

#include "known_window.hpp"

#include <QSystemTrayIcon>

#include <QDialog>
#include <sdbus-c++/IProxy.h>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QEventLoop;
QT_END_NAMESPACE

class Tray : public QDialog {
    Q_OBJECT

  public:
    Tray(iwd &manager);

    void setVisible(bool visible) override;

  private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

  private:
    iwd &manager;

    device cur_device;

    void createTray();
    void instantiateDevice();
    QIcon addNetwork(network n);

    QIcon processConnectedNetwork(network n);
    void refreshNetworks(bool);
    void makeAgent();

    std::string requestPassphrase(const std::string& path);
    std::tuple<std::string,std::string> requestUserAndPassphrase(const std::string& path);

    void createItems();
    void fillMenu();
    void createKnownWindow();

    void connectedHandler(network n, QIcon icon);

    std::unique_ptr<sdbus::IProxy> saved_proxy;
    QMenu *networksMenu = NULL;

    QAction *avoidScans;
    QAction *scanAction;
    QAction *knownAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    KnownWindow *kwindow;

    QSettings settings;
};

#endif
