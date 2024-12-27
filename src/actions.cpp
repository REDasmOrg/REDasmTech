#include "actions.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/detaildialog.h"
#include "dialogs/gotodialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/tabledialog.h"
#include "fontawesome.h"
#include "mainwindow.h"
#include "models/referencesmodel.h"
#include "utils.h"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QHash>
#include <QInputDialog>

namespace actions {

namespace {

QHash<Type, QAction*> g_actions;
MainWindow* g_mainwindow;

const QString DETAIL_TEMPLATE = QString{R"(
    <b>Address:</b> %1<br>
    <b>Offset:</b> %2<br>
    <b>Index:</b> %3<br>
    <b>Byte:</b> %4<br>
    <br>
    )"};

const QString INSTR_TEMPLATE = QString{R"(
        <b>==== INSTRUCTION ====</b><br>
        <b>Address:</b> %1<br>
        <b>Id:</b> %2<br>
        <b>Features:</b> %3<br>
        <b>Length:</b> %4<br>
        <b>Delayslots:</b> %5<br>
        <b>Uservalue:</b> %6<br>
        <br>
        )"};

const QString OP_TEMPLATE = QString{R"(
        <b>==== OPERAND %1 ====</b><br>
        <b>Type:</b> %2<br>
        <b>Userdata1:</b> %3<br>
        <b>Userdata2:</b> %4<br>
        )"};

void append_separated(QString& s, const QString& arg) {
    if(!s.isEmpty())
        s.append(" | ");

    s.append(arg);
}

QString optype_tostring(const RDOperand* op) {
    switch(op->type) {
        case OP_REG: return "OP_REG";
        case OP_IMM: return "OP_IMM";
        case OP_ADDR: return "OP_ADDR";
        case OP_MEM: return "OP_MEM";
        case OP_PHRASE: return "OP_PHRASE";
        case OP_DISPL: return "OP_DISPL";
        default: break;
    }

    return QString{"#%1"}.arg(op->type);
}

QString instrfeatures_tostring(const RDInstruction* instr) {
    QString f;

    if(instr->features & IF_JUMP)
        append_separated(f, "IF_JUMP");

    if(instr->features & IF_CALL)
        append_separated(f, "IF_CALL");

    if(instr->features & IF_STOP)
        append_separated(f, "IF_STOP");

    if(instr->features & IF_DSLOT)
        append_separated(f, "IF_DSLOT");

    if(f.isEmpty())
        f = "IF_NONE";

    return f;
}

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

void show_details() {
    ContextView* cv = g_mainwindow->context_view();
    if(!cv)
        return;

    RDSurfaceLocation loc{};
    rdsurface_getlocation(cv->handle(), &loc);

    RDAddress address;

    if(loc.cursoraddress.valid)
        address = loc.cursoraddress.value;
    else if(loc.address.valid)
        address = loc.address.value;
    else
        return;

    MIndex index;

    if(!rd_addresstoindex(address, &index))
        return;

    const RDByte* bytes;
    usize nbytes = rd_getbytes(&bytes);

    if(index >= nbytes)
        return;

    RDByte b = bytes[index];

    RDOffset offset;
    bool hasoffset = rd_addresstooffset(address, &offset);

    QString s = DETAIL_TEMPLATE.arg(rd_tohex(address))
                    .arg(hasoffset ? rd_tohex(offset) : "N/A")
                    .arg(rd_tohex(index))
                    .arg(rd_tohex_n(b, 8));

    RDInstruction instr;

    if(rdbyte_iscode(b) && rd_decode(address, &instr)) {
        QString strinstr = INSTR_TEMPLATE.arg(rd_tohex(instr.address))
                               .arg(rd_tohex(instr.id))
                               .arg(instrfeatures_tostring(&instr))
                               .arg(rd_tohex(instr.length))
                               .arg(instr.delayslots)
                               .arg(rd_tohex(instr.uservalue));

        foreach_operand(i, op, &instr) {
            QString strop = OP_TEMPLATE.arg(i)
                                .arg(optype_tostring(op))
                                .arg(rd_tohex(op->userdata1))
                                .arg(rd_tohex(op->userdata2));

            switch(op->type) {
                case OP_REG: {
                    strop.append(QString("<b>reg:</b> %1<br>").arg(op->reg));
                    break;
                }

                case OP_IMM: {
                    strop.append(
                        QString("<b>imm:</b> %1<br>").arg(rd_tohex(op->imm)));
                    break;
                }

                case OP_ADDR: {
                    strop.append(
                        QString("<b>addr:</b> %1<br>").arg(rd_tohex(op->addr)));
                    break;
                }

                case OP_MEM: {
                    strop.append(
                        QString("<b>mem:</b> %1<br>").arg(rd_tohex(op->mem)));
                    break;
                }

                case OP_PHRASE: {
                    strop.append(QString("<b>base:</b> %1<br>")
                                     .arg(rd_tohex(op->phrase.base)));
                    strop.append(QString("<b>index:</b> %1<br>")
                                     .arg(rd_tohex(op->phrase.index)));
                    break;
                }

                case OP_DISPL: {
                    strop.append(QString("<b>base:</b> %1<br>")
                                     .arg(rd_tohex(op->displ.base)));
                    strop.append(QString("<b>index:</b> %1<br>")
                                     .arg(rd_tohex(op->displ.index)));
                    strop.append(QString("<b>displ:</b> %1<br>")
                                     .arg(rd_tohex(op->displ.displ)));
                    break;
                }

                default: {
                    strop.append(QString("<b>reg1:</b> %1<br>")
                                     .arg(rd_tohex(op->user.reg1)));
                    strop.append(QString("<b>reg2:</b> %1<br>")
                                     .arg(rd_tohex(op->user.reg2)));
                    strop.append(QString("<b>val:</b> %1<br>")
                                     .arg(rd_tohex(op->user.val)));
                    break;
                }
            }

            strinstr.append(strop);
        }

        s.append(strinstr);
    }

    auto* dlg = new DetailDialog(cv);
    dlg->set_html(s);
    dlg->show();
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

    g_actions[Type::OPEN_DETAILS] = mw->addAction(
        FA_ICON(0x3f), "Details", mw, []() { actions::show_details(); });

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
