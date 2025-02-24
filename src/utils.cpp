#include "utils.h"
#include "actions.h"
#include "dialogs/detaildialog.h"
#include "settings.h"
#include "themeprovider.h"
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>
#include <climits>

namespace utils {

namespace {

float cellw{}, cellh{};

}

float cell_width() {
    if(!utils::cellw) {
        QFontMetricsF fm{REDasmSettings::font()};
        utils::cellw = fm.horizontalAdvance(" ");
    }

    return utils::cellw;
}

float cell_height() {
    if(!utils::cellh) {
        QFontMetricsF fm{REDasmSettings::font()};
        utils::cellh = fm.height();
    }

    return utils::cellh;
}

QString to_hex_addr(RDAddress address, const RDSegment* seg) {
    if(!seg) seg = rd_findsegment(address);

    QString s = QString::number(address, 16).toUpper();
    if(seg) return s.rightJustified(seg->bits / 4, '0');
    return s;
}

void show_detail(const QString& html, QWidget* parent) {
    auto* dlg = new DetailDialog(parent);
    dlg->set_html(html);
    dlg->show();
}

usize draw_surface(RDSurface* s, QTextDocument* doc, usize start, usize n) {
    doc->clear();
    QTextCursor cursor(doc);
    cursor.beginEditBlock();
    usize nmaxcol = 0;

    for(usize i = 0; i < n; i++) {
        if(i) // Insert new block after the default one
            cursor.insertBlock();

        // cursor.insertText(QString::number(i + 1) + " ");

        const RDSurfaceCell* row = nullptr;
        usize maxcol;
        usize ncols = rdsurface_getrow(s, start++, &row, &maxcol);
        nmaxcol = std::max(nmaxcol, maxcol);

        for(usize j = 0; j < ncols; j++) {
            QTextCharFormat cf;
            QColor fg = themeprovider::color(row[j].fg);
            QColor bg = themeprovider::color(row[j].bg);

            if(fg.isValid()) cf.setForeground(fg);
            if(bg.isValid()) cf.setBackground(bg);

            cursor.insertText(QChar{row[j].ch}, cf);
        }
    }

    cursor.endEditBlock();
    return nmaxcol;
}

QMenu* create_surface_menu(RDSurface* s, QWidget* w) {
    QAction* actcopy = actions::get(actions::COPY);
    QAction* actrefs = actions::get(actions::REFS_TO);
    QAction* actrename = actions::get(actions::RENAME);
    QAction* actcomment = actions::get(actions::COMMENT);

    auto* menu = new QMenu(w);
    menu->addAction(actcopy);
    menu->addAction(actrefs);
    menu->addAction(actrename);
    menu->addSeparator();
    menu->addAction(actcomment);
    menu->addAction(actions::get(actions::GOTO));
    menu->addSeparator();
    menu->addAction(actions::get(actions::OPEN_DETAILS));

    QObject::connect(menu, &QMenu::aboutToShow, w, [=]() {
        RDSurfaceLocation loc{};
        rdsurface_getlocation(s, &loc);

        actcomment->setVisible(
            loc.listingindex.valid &&
            (loc.listingindex.type == LISTINGITEM_INSTRUCTION ||
             loc.listingindex.type == LISTINGITEM_TYPE));

        actcopy->setVisible(rdsurface_hasselection(s));
        actrename->setVisible(loc.cursoraddress.valid);
        actrefs->setVisible(loc.cursoraddress.valid &&
                            rd_getrefsto(loc.cursoraddress.value, nullptr));
    });

    return menu;
}

bool handle_key_press(RDSurface* s, QKeyEvent* e) {
    RDSurfacePosition pos;
    rdsurface_getposition(s, &pos);

    auto [row, col] = pos;

    if(e->matches(QKeySequence::MoveToNextChar)) {
        rdsurface_setposition(s, row, col + 1);
    }
    else if(e->matches(QKeySequence::MoveToPreviousChar)) {
        if(col > 0) rdsurface_setposition(s, row, col - 1);
    }
    else if(e->matches(QKeySequence::MoveToNextLine)) {
        rdsurface_setposition(s, row + 1, col);
    }
    else if(e->matches(QKeySequence::MoveToPreviousLine)) {
        if(row > 0) rdsurface_setposition(s, row - 1, col);
    }
    else if(e->matches(QKeySequence::MoveToStartOfLine)) {
        rdsurface_setposition(s, row, 0);
    }
    else if(e->matches(QKeySequence::SelectNextChar)) {
        rdsurface_select(s, row, col + 1);
    }
    else if(e->matches(QKeySequence::SelectPreviousChar)) {
        if(col > 0) rdsurface_select(s, row, col - 1);
    }
    else if(e->matches(QKeySequence::SelectNextLine)) {
        rdsurface_select(s, row + 1, col);
    }
    else if(e->matches(QKeySequence::SelectPreviousLine)) {
        if(row > 0) rdsurface_select(s, row - 1, col);
    }
    else if(e->matches(QKeySequence::SelectStartOfLine)) {
        rdsurface_select(s, row, 0);
    }
    else if(e->matches(QKeySequence::SelectStartOfDocument)) {
        rdsurface_select(s, 0, 0);
    }
    else
        return false;

    return true;
}

} // namespace utils
