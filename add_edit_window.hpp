#ifndef AEWINDOW_HPP
#define AEWINDOW_HPP

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

class AddEditWindow: public QDialog{
    Q_OBJECT

    public:
        AddEditWindow(const known_network *n = nullptr, QWidget *parent = nullptr);
        
    protected:
        //void closeEvent(QCloseEvent *event) override;

    private:
        QVBoxLayout *layout;         

        QLabel *networkLabel = NULL;
        QLabel *typeLabel = NULL;
        QLabel *autoconnectLabel = NULL;

        QLineEdit *networkName = NULL;
        QComboBox *networkTypes = NULL;
        QCheckBox *autoconnectEnabled = NULL;

        QLabel *passwordLabel = NULL;
        QLineEdit *networkPassword = NULL;

        QLabel *eapMethodLabel = NULL;
        QComboBox *eapMethod = NULL;
        QLabel *eapIdentityLabel = NULL;
        QLineEdit *eapIdentity = NULL;
        QLabel *eapPhase2IdentityLabel = NULL;
        QLineEdit *eapPhase2Identity = NULL;
        QLabel *eapPhase2MethodLabel = NULL;
        QComboBox *eapPhase2Method = NULL;


        QPushButton *saveButton = NULL;

        QStringList supportedTypes();
        QStringList supportedEAPMethods();
        QStringList supportedPhase2Methods();

        void setFlags();
        void newWindow();
        void refreshWindow();
        void createItems();
        void fillUpFromNetwork(const known_network *);
};

#endif
