#include "utils.h"
#include "settings.h"
#include "themeprovider.h"
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>
#include <climits>

namespace utils {

namespace {

float cellw{}, cellh{};

}

QString to_hex(size_t n) {
    const int N_CHARS = (utils::bits / CHAR_BIT) * 2;
    return QStringLiteral("%1").arg(n, N_CHARS, 16, QLatin1Char('0')).toUpper();
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

void draw_surface(RDSurface* s, QTextDocument* doc, usize start, usize n) {
    doc->clear();
    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    for(usize i = start; i < n; i++) {
        const RDSurfaceCell* row = nullptr;
        usize ncols = rdsurface_getrow(s, i, &row);

        for(usize j = 0; j < ncols; j++) {
            QTextCharFormat cf;
            QColor fg = themeprovider::color(row[j].fg);
            QColor bg = themeprovider::color(row[j].bg);

            if(fg.isValid())
                cf.setForeground(fg);
            if(bg.isValid())
                cf.setBackground(bg);

            cursor.insertText(QChar{row[j].ch}, cf);
        }

        cursor.insertBlock();
    }

    cursor.endEditBlock();
}

} // namespace utils
