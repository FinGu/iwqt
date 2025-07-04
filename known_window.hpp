#ifndef KWINDOW_HPP
#define KWINDOW_HPP

#include "iwd.hpp"
#include "network.hpp"

#include "manage_window.hpp"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(known_network);

class KnownWindow: public QDialog{
    Q_OBJECT

    public:
        KnownWindow(iwd &manager, QWidget *parent = nullptr);
        
    protected:
        void closeEvent(QCloseEvent *event) override;

    private:
        iwd &manager;

        QListWidget *listWidget;
        QVBoxLayout *layout;         
        QPushButton *refreshButton;
        QPushButton *addButton;

        ManageWindow *mwindow;

        void setFlags();
        void createItems();
        void refreshNetworks();
};

#endif
