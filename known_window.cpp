#include "known_window.hpp"

#include "network.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QToolTip>
#include <QMenu>
#include <QCloseEvent>

#include <sdbus-c++/Types.h>

enum KnownRoles{
    Show = Qt::UserRole, 
    Manage
};

KnownWindow::KnownWindow(iwd &manager, QWidget *parent): QDialog(parent), manager(manager){
    setFlags();

    createItems();

    refreshNetworks();

    connect(refreshButton, &QPushButton::clicked, this, [=]{
        refreshNetworks(); 
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

void KnownWindow::refreshNetworks(){
    listWidget->clear();

    auto inetworks = this->manager.known_networks();

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

void KnownWindow::createItems(){
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

