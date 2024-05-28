#include "actions.h"
// #include "fontawesome.h"
#include "dialogs/gotodialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QClipboard>
#include <QHash>

namespace actions {

namespace {

QHash<Type, QAction*> g_actions;
MainWindow* g_mainwindow;

void show_goto() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    auto* dlggoto = new GotoDialog(g_mainwindow);

    QObject::connect(
        dlggoto, &GotoDialog::accepted, g_mainwindow,
        [&, cv, dlggoto]() { cv->surface_view()->jump_to(dlggoto->address); });

    dlggoto->show();
}

void copy() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    qApp->clipboard()->setText(
        rdsurface_getselectedtext(cv->surface_view()->handle()));
}

} // namespace

void init(MainWindow* mw) {
    g_mainwindow = mw;

    g_actions[Type::GOTO] = mw->addAction("Goto…", QKeySequence(Qt::Key_G), mw,
                                          []() { actions::show_goto(); });

    g_actions[Type::COPY] =
        mw->addAction("Copy", QKeySequence(Qt::CTRL | Qt::Key_C), mw,
                      []() { actions::copy(); });
}

QAction* get(Type t) {
    if(auto it = g_actions.find(t); it != g_actions.end())
        return it.value();

    return nullptr;
}

} // namespace actions
