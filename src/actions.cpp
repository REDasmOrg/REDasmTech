#include "actions.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/gotodialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/tabledialog.h"
#include "fontawesome.h"
#include "mainwindow.h"
#include "models/referencesmodel.h"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QHash>
#include <QInputDialog>

namespace actions {

namespace {

QHash<Type, QAction*> g_actions;
MainWindow* g_mainwindow;

void show_goto() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    auto* dlggoto = new GotoDialog(cv);

    QObject::connect(dlggoto, &GotoDialog::accepted, g_mainwindow,
                     [&, cv, dlggoto]() { cv->jump_to(dlggoto->address); });

    dlggoto->show();
}

void copy() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    qApp->clipboard()->setText(rdsurface_getselectedtext(cv->handle()));
}

void comment() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    RDSurfaceLocation loc{};
    rdsurface_getlocation(cv->handle(), &loc);
    if(!loc.address.valid)
        return;

    bool ok = false;
    const char* cmt = rd_getcomment(loc.address.value);

    QString s = QInputDialog::getMultiLineText(
        g_mainwindow, QString{"Comment @ %1"}.arg(rd_tohex(loc.address.value)),
        "Insert comment (or leave empty):", cmt ? cmt : QString{}, &ok);

    if(ok && rd_setcomment(loc.address.value, qUtf8Printable(s)))
        cv->invalidate();
}

void refs_to() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    RDAddress address;
    if(!rdsurface_getaddressundercursor(cv->handle(), &address))
        return;

    auto* dlg =
        new TableDialog(QString("References to %1").arg(rd_tohex(address)), cv);

    QObject::connect(dlg, &TableDialog::double_clicked, g_mainwindow,
                     [cv, dlg](const QModelIndex& index) {
                         auto* m = static_cast<ReferencesModel*>(dlg->model());
                         cv->jump_to(m->address(index));
                         dlg->accept();
                     });

    dlg->set_model(new ReferencesModel(address, dlg));
    dlg->show();
}

void rename() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    RDAddress address;
    if(!rdsurface_getaddressundercursor(cv->handle(), &address))
        return;

    QString name;

    if(const char* s = rd_getname(address); s)
        name = s;

    bool ok = false;
    QString s = QInputDialog::getText(
        g_mainwindow, QString("Rename @ %1").arg(rd_tohex(address)), "New Name",
        QLineEdit::Normal, name, &ok);

    if(ok && rd_setname(address, qUtf8Printable(s)))
        cv->invalidate();
}

} // namespace

void init(QMainWindow* mw) {
    g_mainwindow = static_cast<MainWindow*>(mw);

    g_actions[Type::GOTO] =
        mw->addAction(FA_ICON(0xf1e5), "Goto…", QKeySequence{Qt::Key_G}, mw,
                      []() { actions::show_goto(); });

    g_actions[Type::COPY] =
        mw->addAction("Copy", QKeySequence{Qt::CTRL | Qt::Key_C}, mw,
                      []() { actions::copy(); });

    g_actions[Type::REFS_TO] =
        mw->addAction("Cross References To…", QKeySequence{Qt::Key_X}, mw,
                      []() { actions::refs_to(); });

    g_actions[Type::RENAME] = mw->addAction("Rename", QKeySequence(Qt::Key_N),
                                            mw, []() { actions::rename(); });

    g_actions[Type::COMMENT] =
        mw->addAction("Comment", QKeySequence{Qt::Key_Semicolon}, mw,
                      []() { actions::comment(); });

    g_actions[Type::OPEN_HOME] =
        mw->addAction(FA_ICON(0xf015), "Homepage", mw, []() {
            QDesktopServices::openUrl(QUrl{"https://redasm.dev"});
        });

    g_actions[Type::OPEN_TELEGRAM] =
        mw->addAction(FAB_ICON(0xf2c6), "Telegram", mw, []() {
            QDesktopServices::openUrl(QUrl{"https://t.me/REDasmDisassembler"});
        });

    g_actions[Type::OPEN_REDDIT] =
        mw->addAction(FAB_ICON(0xf281), "Reddit", mw, []() {
            QDesktopServices::openUrl(QUrl{"https://reddit.com/r/REDasm"});
        });

    g_actions[Type::OPEN_X] = mw->addAction(FAB_ICON(0xe61b), "X", mw, []() {
        QDesktopServices::openUrl(QUrl{"https://x.com/re_dasm"});
    });

    g_actions[Type::OPEN_GITHUB] =
        mw->addAction(FAB_ICON(0xf113), "Report an Issue", mw, []() {
            QDesktopServices::openUrl(
                QUrl{"https://github.com/REDasmOrg/REDasm/issues"});
        });

    g_actions[Type::OPEN_ABOUT] = mw->addAction("&About", mw, []() {
        auto* dlgabout = new AboutDialog(g_mainwindow);
        dlgabout->show();
    });

    g_actions[Type::OPEN_SETTINGS] = mw->addAction("&Settings", mw, []() {
        auto* dlgsettings = new SettingsDialog(g_mainwindow);
        dlgsettings->show();
    });
}

QAction* get(Type t) {
    if(auto it = g_actions.find(t); it != g_actions.end())
        return it.value();

    return nullptr;
}

} // namespace actions
