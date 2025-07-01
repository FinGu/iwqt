#ifndef WINDOW_H
#define WINDOW_H

#include "iwd.hpp"

#include "device.hpp"

#include <QSystemTrayIcon>

#include <QDialog>

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


class Window : public QDialog {
    Q_OBJECT

  public:
    Window(iwd &manager);

    void setVisible(bool visible) override;

  private slots:

  private:
    iwd &manager;

    device cur_device;

    void createTray();
    void instantiateDevice();
    void addNetwork(network n);
    void refreshNetworks(bool);
    void processConnectedNetwork(network n);
    void makeAgent();

    void iwdNotUp();

    std::string requestPassphrase(const std::string& path);
    std::tuple<std::string,std::string> requestUserAndPassphrase(const std::string& path);

    void createItems();
    void fillMenu();

    QMenu *networksMenu = NULL;

    QAction *avoidScans;
    QAction *scanAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif
