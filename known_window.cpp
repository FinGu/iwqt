#include "known_window.hpp"

#include "network.hpp"

#include "utils.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QToolTip>
#include <QMenu>
#include <QCloseEvent>
#include <QToolButton>
#include <QActionGroup>
#include <QDateTime>
#include <QSettings>

#include <cctype>
#include <sdbus-c++/Types.h>

constexpr auto SORT_ICON_PATH = ":/images/sort.png";

enum KnownRoles{
    Show = Qt::UserRole, 
    Manage
};

KnownWindow::KnownWindow(iwd &manager, QWidget *parent): QDialog(parent), manager(manager){
    setWindowIcon(QPixmap(EXCELLENT_ICON_PATH));

    setFlags();

    createItems();

    refreshNetworks();

    connect(refreshButton, &QPushButton::clicked, this, [=]{
        try{
            refreshNetworks(); 
        } catch(...){
            //we might as well just ignore, handle it in the tray
        }
    });

    connect(addButton, &QPushButton::clicked, this, [=]{
        ManageWindow *win = new ManageWindow(nullptr, this);
        win->exec();
        refreshNetworks();
    });
    
    connect(listWidget, &QListWidget::itemEntered, this,
            [=](QListWidgetItem* it){
        QString info = it->data(KnownRoles::Show).toString();

        auto pos = QCursor::pos();

        pos.setY(pos.y() - 5);

        QToolTip::showText(pos, info, listWidget);
    });

    setLayout(layout);
    setMinimumWidth(350);
}

void KnownWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape){
        event->ignore();
        return;
    }
}

void KnownWindow::closeEvent(QCloseEvent *event){
    hide();
    event->ignore();
}

void KnownWindow::sortNetworks(std::vector<known_network> &nets){
    switch(currentSortMethod){
        case SortType::ByName:
            std::sort(nets.begin(), nets.end(), [](known_network &a, known_network &b){
                return QString::fromStdString(a.name).toLower() < QString::fromStdString(b.name).toLower();
            });
            break;
        case SortType::ByLast:
            std::sort(nets.begin(), nets.end(), [](known_network &a, known_network &b){
                auto dtA = QDateTime::fromString(QString::fromStdString(a.last_connected),
                                         Qt::ISODate);
                auto dtB = QDateTime::fromString(QString::fromStdString(b.last_connected),
                                         Qt::ISODate);

                return dtA.toSecsSinceEpoch() > dtB.toSecsSinceEpoch();
            });
            break;
        case SortType::ByType:
            std::sort(nets.begin(), nets.end(), [](known_network &a, known_network &b){
                return QString::fromStdString(a.type).toLower() < QString::fromStdString(b.type).toLower();
            });
            break;
        default:
            break;
    }
}

void KnownWindow::refreshNetworks(){
    listWidget->clear();

    auto inetworks = this->manager.known_networks();

    sortNetworks(inetworks);

    for(auto n: inetworks){
        auto *it = new QListWidgetItem(n.name.c_str());
        
        QString to_show = QString("Type: %1\nHidden: %2\nAutoconnect: %3\nLast connected: %4")
        .arg(n.type)
        .arg(n.hidden ? "true" : "false")
        .arg(n.autoconnect ? "true" : "false")
        .arg(n.last_connected);

        it->setData(KnownRoles::Show, to_show);
        
        QVariant data;

        data.setValue(n);

        it->setData(KnownRoles::Manage, data);
        
        listWidget->addItem(it);
    }
}

QMenu *KnownWindow::createSortItems(){
    auto *menu = new QMenu(sortButton);

    auto *group = new QActionGroup(menu);

    auto addAction = [=](const char *val){
        QAction *p = menu->addAction(tr(val));

        group->addAction(p);
        p->setCheckable(true);

        return p;
    };

    QAction *name = addAction("By name");
    connect(name, &QAction::triggered, this, [this](){
        currentSortMethod = SortType::ByName;
        settings.setValue(SORT_SETTING, (int)currentSortMethod);
        refreshNetworks();
    });

    QAction *last = addAction("By last connected");
    connect(last, &QAction::triggered, this, [this](){
        currentSortMethod = SortType::ByLast;
        settings.setValue(SORT_SETTING, (int)currentSortMethod);
        refreshNetworks();
    });

    QAction *type = addAction("By type");
    connect(type, &QAction::triggered, this, [this](){
        currentSortMethod = SortType::ByType;
        settings.setValue(SORT_SETTING, (int)currentSortMethod);
        refreshNetworks();
    });

    auto sortType = (SortType)settings.value(SORT_SETTING, 0).toInt();

    switch(sortType){
        case SortType::ByName:
            name->setChecked(true);
            break;
        case SortType::ByLast:
            last->setChecked(true);
            break;
        case SortType::ByType:
            last->setChecked(true);
            break;
    }

    currentSortMethod = sortType;

    return menu;
}

void KnownWindow::createItems(){
    sortButton = new QToolButton(this);

    sortButton->setFixedSize(15, 15);
    sortButton->setIconSize(QSize(15, 15));
    sortButton->setIcon(QIcon(SORT_ICON_PATH));

    sortButton->setStyleSheet(QString{"QToolButton {border: 0px; margin-top: -3px;} QToolButton::menu-indicator { image: none; }"});

    sortButton->setPopupMode(QToolButton::InstantPopup);

    sortButton->setMenu(createSortItems());

    listWidget = new QListWidget(this);
    listWidget->setMouseTracking(true);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(listWidget, &QWidget::customContextMenuRequested,
        this, [&](const QPoint &pos){
            QMenu menu(this);

            QAction *editAct = menu.addAction("Edit");
            QAction *deleteAct = menu.addAction("Forget");
    
            QAction *chosen = menu.exec(QCursor::pos());

            if (!chosen) {
                return;
            }

            auto cur = listWidget->indexAt(pos);
            auto data = cur.data(KnownRoles::Manage).value<known_network>();

            if (chosen == editAct) {
                ManageWindow *win = new ManageWindow(&data, this);
                win->exec();
                refreshNetworks(); //in case autoconnect is changed
            } else if (chosen == deleteAct) {
                this->manager.forget_known_network(data);

                delete listWidget->takeItem(cur.row());
            }
    });

    refreshButton = new QPushButton("Refresh", this);
    refreshButton->setFixedSize(95, 25);

    addButton = new QPushButton("Add new", this);
    addButton->setFixedSize(95, 25);
      
    layout = new QVBoxLayout(this);

    layout->addWidget(sortButton, 0, Qt::AlignRight);

    layout->addWidget(listWidget);

    QHBoxLayout *layout2 = new QHBoxLayout();
    
    layout2->addStretch();
    layout2->addWidget(refreshButton);
    layout2->addWidget(addButton);

    layout->addLayout(layout2);
}

void KnownWindow::setFlags(){
    Qt::WindowFlags flags = windowFlags();
    
    flags &= ~Qt::WindowFullscreenButtonHint;
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinimizeButtonHint;

    setWindowFlags(flags);
}

