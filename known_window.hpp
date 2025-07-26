#ifndef KWINDOW_HPP
#define KWINDOW_HPP

#include "iwd.hpp"
#include "network.hpp"

#include "manage_window.hpp"

#include <QDialog>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
class QToolButton;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(known_network);

enum SortType{
    ByName = 0,
    ByLast, 
    ByType
};

class KnownWindow: public QDialog{
    Q_OBJECT

    public:
        KnownWindow(iwd &manager, QWidget *parent = nullptr);
        
    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void closeEvent(QCloseEvent *event) override;

    private:
        iwd &manager;

        QToolButton *sortButton;

        QListWidget *listWidget;
        QVBoxLayout *layout;         

        QPushButton *refreshButton;
        QPushButton *addButton;

        ManageWindow *mwindow;

        void setFlags();
        void createItems();
        QMenu *createSortItems();
        void refreshNetworks();
        void sortNetworks(std::vector<known_network>&);

        SortType currentSortMethod = SortType::ByName;
        QSettings settings;
};

#endif
