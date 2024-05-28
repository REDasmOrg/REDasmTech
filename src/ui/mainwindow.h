#pragma once

#include "../fontawesome.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>

namespace ui {

struct MainWindow {
    QStatusBar* statusbar;
    QStackedWidget* stackwidget;
    QAction *actfileopen, *actfileclose, *actfileexit;
    QAction *actviewmemorymap, *actviewsegments, *actviewstrings;
    QAction *acttoolsflc, *acttoolssettings;
    QAction *actedit, *actview, *acttools;
    QMenu *mnufile, *mnuedit, *mnuview, *mnutools, *mnuhelp;
    QMenu* mnurecents;

    explicit MainWindow(QMainWindow* self) {
        self->setAcceptDrops(true);
        self->resize(1300, 850);

#if !defined(NDEBUG)
        self->setFixedSize(1300, 850);
#endif

        auto* menubar = new QMenuBar(self);
        this->mnufile = menubar->addMenu("&File");
        this->mnuedit = menubar->addMenu("&Edit");
        this->mnuview = menubar->addMenu("&View");
        this->mnutools = menubar->addMenu("&Tools");
        this->mnuhelp = menubar->addMenu("&?");

        this->actedit = this->mnuedit->menuAction();
        this->actedit->setVisible(false);

        this->actview = this->mnuview->menuAction();
        this->actview->setVisible(false);

        this->acttools = this->mnutools->menuAction();
        this->acttools->setVisible(false);

        this->actfileopen = this->mnufile->addAction(FA_ICON(0xf07c), "&Open");
        this->mnurecents = new QMenu("&Recent Files", menubar);
        this->mnufile->addMenu(this->mnurecents);

        this->actfileclose = this->mnufile->addAction("Close");
        this->actfileclose->setVisible(false);

        this->mnufile->addSeparator();
        this->actfileexit = this->mnufile->addAction("&Exit");

        this->acttoolsflc = this->mnutools->addAction(
            "&FLC", QKeySequence{Qt::CTRL | Qt::Key_L});

        this->acttoolsflc->setVisible(false);
        this->acttoolssettings = this->mnutools->addAction("&Settings");

        this->actviewmemorymap = this->mnuview->addAction(
            "&Memory Map", QKeySequence{Qt::CTRL | Qt::Key_M});

        this->actviewsegments = this->mnuview->addAction(
            "S&egments", QKeySequence{Qt::CTRL | Qt::SHIFT | Qt::Key_S});

        this->actviewstrings = this->mnuview->addAction(
            "&Strings", QKeySequence{Qt::CTRL | Qt::Key_S});

        auto* toolbar = new QToolBar(self);
        toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        toolbar->setFloatable(false);
        toolbar->setMovable(false);
        toolbar->addAction(this->actfileopen);
        self->addToolBar(toolbar);

        self->setMenuBar(menubar);

        this->statusbar = new QStatusBar(self);
        self->setStatusBar(this->statusbar);

        this->stackwidget = new QStackedWidget();
        self->setCentralWidget(this->stackwidget);
    }
};

} // namespace ui
