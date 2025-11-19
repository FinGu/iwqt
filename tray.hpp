#ifndef TRAY_HPP
#define TRAY_HPP

#include "iwd.hpp"

#include "adapter.hpp"
#include "device.hpp"

#include "manage_window.hpp"

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

    bool isDarkMode;

    adapter cur_adapter;
    device cur_device;

    void createTray();
    void instantiateDevice();
    QPixmap addNetwork(network n);

    QPixmap processConnectedNetwork(network n);
    void updateEnabledTray(bool);
    void refreshTray(bool);
    void makeAgent();

    std::string requestPassphrase(const std::string& path);
    std::tuple<std::string,std::string> requestUserAndPassphrase(const std::string& path);

    void createItems();
    void fillMenu();
    void createManageWindow();

    void connectedHandler(network n, QPixmap icon);

    QPixmap getIconForStrength(network::strength_type st);

    std::unique_ptr<sdbus::IProxy> saved_proxy;
    QMenu *networksMenu = NULL;

    QAction *enabledAdapterAction;
    QAction *scanAction;
    QAction *manageAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    ManageWindow *mwindow;

    QSettings settings;
};

#endif
