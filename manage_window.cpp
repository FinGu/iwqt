#include "manage_window.hpp"

#include "add_edit_window.hpp"
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
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include <cctype>
#include <sdbus-c++/Types.h>

enum KnownRoles{
    Show = Qt::UserRole, 
    Manage
};

ManageWindow::ManageWindow(iwd &manager, QWidget *parent): QDialog(parent), manager(manager){
    setWindowIcon(Utils::getIcon(EXCELLENT_ICON_PATH));

    setFlags();

    createItems();

    refreshNetworks();

    if(settings.value(AVOID_SCANS_SETTING, false).toBool()){
        avoidScansCheckbox->setChecked(true);
    }

    connect(avoidScansCheckbox, &QCheckBox::checkStateChanged, this, [=, this]{
        settings.setValue(AVOID_SCANS_SETTING, avoidScansCheckbox->isChecked());
    });

    if(settings.value(SHOW_NOTIFICATIONS_SETTING, true).toBool()){
        showNotificationsCheckbox->setChecked(true);
    }

    connect(showNotificationsCheckbox, &QCheckBox::checkStateChanged, this, [=, this]{
        settings.setValue(SHOW_NOTIFICATIONS_SETTING, showNotificationsCheckbox->isChecked());
    });

    connect(refreshButton, &QPushButton::clicked, this, [=, this]{
        try{
            refreshNetworks(); 
        } catch(...){
            //we might as well just ignore, handle it in the tray
        }
    });

    connect(addButton, &QPushButton::clicked, this, [=, this]{
        AddEditWindow *win = new AddEditWindow(nullptr, this);
        win->exec();
        refreshNetworks();
    });
    
    connect(listWidget, &QListWidget::itemEntered, this,
            [=, this](QListWidgetItem* it){
        QString info = it->data(KnownRoles::Show).toString();

        auto pos = QCursor::pos();

        pos.setY(pos.y() - 5);

        QToolTip::showText(pos, info, listWidget);
    });

    connect(this, &ManageWindow::iconThemeChanged, this, [=, this](){
        updateButtonIcons();
    });

    setLayout(layout);
    setMinimumWidth(350);
}

void ManageWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape){
        event->ignore();
        return;
    }
}

void ManageWindow::closeEvent(QCloseEvent *event){
    hide();
    event->ignore();
}

void ManageWindow::sortNetworks(std::vector<known_network> &nets){
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

void ManageWindow::refreshNetworks(){
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

QMenu *ManageWindow::createSortItems(){
    auto *menu = new QMenu(sortButton);

    auto *group = new QActionGroup(menu);

    auto addAction = [=](const char *val){
        QAction *p = menu->addAction(tr(val));

        group->addAction(p);
        p->setCheckable(true);

        return p;
    };

    auto setSort = [this](SortType type){
        currentSortMethod = type;
        settings.setValue(SORT_SETTING, (int)currentSortMethod);
        refreshNetworks();
    };

    QAction *name = addAction("By name");
    connect(name, &QAction::triggered, this, [=](){ setSort(SortType::ByName); });

    QAction *last = addAction("By last connected");
    connect(last, &QAction::triggered, this, [=](){ setSort(SortType::ByLast); });

    QAction *type = addAction("By type");
    connect(type, &QAction::triggered, this, [=](){ setSort(SortType::ByType); });

    auto sortType = (SortType)settings.value(SORT_SETTING, 0).toInt();

    switch(sortType){
        case SortType::ByName:
            name->setChecked(true);
            break;
        case SortType::ByLast:
            last->setChecked(true);
            break;
        case SortType::ByType:
            type->setChecked(true);
            break;
    }

    currentSortMethod = sortType;

    return menu;
}

QMenu *ManageWindow::createThemeItems(){
    auto *menu = new QMenu(themeButton);

    auto *group = new QActionGroup(menu);

    auto addAction = [=](const char *val){
        QAction *p = menu->addAction(tr(val));

        group->addAction(p);
        p->setCheckable(true);

        return p;
    };

    auto setTheme = [this](const QString &theme) {
        settings.setValue(ICON_THEME_SETTING, theme);
        emit iconThemeChanged();
    };

    QAction *aut = addAction("Auto");
    connect(aut, &QAction::triggered, this, [=](){ setTheme(ICON_THEME_AUTO); });

    QAction *dark = addAction("Dark panel");
    connect(dark, &QAction::triggered, this, [=](){ setTheme(ICON_THEME_DARK); });

    QAction *light = addAction("Light panel");
    connect(light, &QAction::triggered, this, [=](){ setTheme(ICON_THEME_LIGHT); });

    auto iconTheme = settings.value(ICON_THEME_SETTING, ICON_THEME_AUTO).toString();

    aut->setChecked(iconTheme == ICON_THEME_AUTO);
    dark->setChecked(iconTheme == ICON_THEME_DARK);
    light->setChecked(iconTheme == ICON_THEME_LIGHT);

    return menu;
}

void ManageWindow::createItems(){
    sortButton = new QToolButton(this);

    sortButton->setFixedSize(15, 15);
    sortButton->setIconSize(QSize(15, 15));
    sortButton->setStyleSheet(QString{"QToolButton {border: 0px; margin-top: -3px;} QToolButton::menu-indicator { image: none; }"});
    sortButton->setPopupMode(QToolButton::InstantPopup);
    sortButton->setMenu(createSortItems());
    
    themeButton = new QToolButton(this);
    themeButton->setFixedSize(15, 15);
    themeButton->setIconSize(QSize(15, 15));
    themeButton->setStyleSheet(QString{"QToolButton {border: 0px; margin-top: -3px;} QToolButton::menu-indicator { image: none; }"});
    themeButton->setPopupMode(QToolButton::InstantPopup);
    themeButton->setMenu(createThemeItems());

    updateButtonIcons();

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
                AddEditWindow *win = new AddEditWindow(&data, this);
                win->exec();
                refreshNetworks(); //in case autoconnect is changed
            } else if (chosen == deleteAct) {
                this->manager.forget_known_network(data);

                delete listWidget->takeItem(cur.row());
            }
    });

    avoidScansCheckbox = new QCheckBox("Avoid scans", this);
    showNotificationsCheckbox = new QCheckBox("Show notifications", this);

    refreshButton = new QPushButton("Refresh", this);
    refreshButton->setFixedSize(95, 25);

    addButton = new QPushButton("Add new", this);
    addButton->setFixedSize(95, 25);
      
    layout = new QVBoxLayout(this);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch();
    layout2->addWidget(themeButton, 0, Qt::AlignRight);
    layout2->addWidget(sortButton, 0, Qt::AlignRight);

    layout->addLayout(layout2);

    layout->addWidget(listWidget);
    
    QHBoxLayout *layout3 = new QHBoxLayout();
    
    layout3->addStretch();
    layout3->addWidget(avoidScansCheckbox, 0, Qt::AlignLeft);
    layout3->addWidget(showNotificationsCheckbox, 0, Qt::AlignLeft);

    layout3->addWidget(refreshButton);
    layout3->addWidget(addButton);

    layout->addLayout(layout3);
}

void ManageWindow::setFlags(){
    Qt::WindowFlags flags = windowFlags();
    
    flags &= ~Qt::WindowFullscreenButtonHint;
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinimizeButtonHint;

    setWindowFlags(flags);
}

void ManageWindow::updateButtonIcons(){
    auto currentIconTheme = settings.value(ICON_THEME_SETTING, ICON_THEME_AUTO).toString();

    if(currentIconTheme != ICON_THEME_DARK && currentIconTheme != ICON_THEME_LIGHT) {
        isDarkMode = Utils::getAutoDarkMode(this);
    } else{
        isDarkMode = currentIconTheme == ICON_THEME_DARK;
    }

    sortButton->setIcon(QIcon(isDarkMode ? SORT_ICON_PATH : DARK_SORT_ICON_PATH));

    themeButton->setIcon(QIcon(isDarkMode ? THEME_ICON_PATH : DARK_THEME_ICON_PATH));
}
