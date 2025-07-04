#ifndef MWINDOW_HPP
#define MWINDOW_HPP

#include "iwd.hpp"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QLabel;
QT_END_NAMESPACE

class ManageWindow: public QDialog{
    Q_OBJECT

    public:
        ManageWindow(const known_network *n = nullptr, QWidget *parent = nullptr);
        
    protected:
        //void closeEvent(QCloseEvent *event) override;

    private:
        QVBoxLayout *layout;         

        QLabel *networkLabel = NULL;
        QLabel *typeLabel = NULL;
        QLabel *autoconnectLabel = NULL;
        QLabel *passwordLabel = NULL;

        QLineEdit *networkName = NULL;
        QComboBox *networkTypes = NULL;
        QCheckBox *autoconnectEnabled = NULL;
        QLineEdit *networkPassword = NULL;

        QStringList supportedTypes();

        void setFlags();
        void newWindow();
        void refreshWindow(bool);
        void delWindow();
        void createItems();
        void fillUpFromNetwork(const known_network *);
};

#endif
