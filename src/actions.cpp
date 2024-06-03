#include "actions.h"
#include "dialogs/gotodialog.h"
#include "dialogs/tabledialog.h"
#include "mainwindow.h"
#include "models/referencesmodel.h"
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

void refs() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    RDAddress address;
    if(!rdsurface_getaddressundercursor(cv->surface_view()->handle(), &address))
        return;

    auto* dlg = new TableDialog(
        QString("References for %1").arg(rd_tohex(address)), g_mainwindow);

    QObject::connect(dlg, &TableDialog::double_clicked, g_mainwindow,
                     [cv, dlg](const QModelIndex& index) {
                         auto* m = static_cast<ReferencesModel*>(dlg->model());
                         cv->surface_view()->jump_to(m->address(index));
                         dlg->accept();
                     });

    dlg->set_model(new ReferencesModel(address, dlg));
    dlg->show();
}

} // namespace

void init(MainWindow* mw) {
    g_mainwindow = mw;

    g_actions[Type::GOTO] = mw->addAction("Goto…", QKeySequence(Qt::Key_G), mw,
                                          []() { actions::show_goto(); });

    g_actions[Type::COPY] =
        mw->addAction("Copy", QKeySequence(Qt::CTRL | Qt::Key_C), mw,
                      []() { actions::copy(); });

    g_actions[Type::REFS] =
        mw->addAction("Cross References", QKeySequence(Qt::Key_X), mw,
                      []() { actions::refs(); });
}

QAction* get(Type t) {
    if(auto it = g_actions.find(t); it != g_actions.end())
        return it.value();

    return nullptr;
}

} // namespace actions
