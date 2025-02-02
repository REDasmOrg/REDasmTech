#include "statusbar.h"
#include "fontawesome.h"
#include "themeprovider.h"
#include <redasm/redasm.h>

namespace statusbar {

namespace {

QLabel* g_lblstatuslabel;
QPushButton* g_btnstatusicon;

} // namespace

QLabel* set_status_label(QLabel* label) {
    g_lblstatuslabel = label;
    return g_lblstatuslabel;
};

QPushButton* set_status_icon(QPushButton* icon, int size) {
    g_btnstatusicon = icon;
    g_btnstatusicon->setFlat(true);
    g_btnstatusicon->setText("\uf0c8");
    g_btnstatusicon->setFont(fontawesome::fa_font());
    g_btnstatusicon->setFixedSize(size, size);
    g_btnstatusicon->hide();
    return g_btnstatusicon;
}

void set_status_text(const QString& s) { g_lblstatuslabel->setText(s); }

void set_location(const RDSurface* surface) {
    RDSurfaceLocation loc;
    rdsurface_getlocation(surface, &loc);

    QString s;

    if(loc.function.valid) {
        s += QString{"<b>Function: </b>%1"}.arg(rd_getname(loc.function.value));

        if(loc.address.valid && (loc.address.value != loc.function.value)) {
            int diff = qMax(loc.address.value, loc.function.value) -
                       qMin(loc.address.value, loc.function.value);

            if(loc.address.value < loc.function.value)
                s += "-";
            else
                s += "+";

            s += QString::number(diff, 16);
        }

        s += QString::fromWCharArray(L"\u00A0\u00A0");
    }

    if(loc.address.valid) {
        s += QString::fromWCharArray(L"<b>Address: </b>%1\u00A0\u00A0")
                 .arg(rd_tohex(loc.address.value));
    }

    if(loc.offset.valid) {
        s += QString::fromWCharArray(L"<b>Offset: </b>%1\u00A0\u00A0")
                 .arg(rd_tohex(loc.offset.value));
    }

    if(loc.segment) {
        s += QString::fromWCharArray(L"<b>Segment: </b>%1\u00A0\u00A0")
                 .arg(loc.segment->name);
    }

    g_lblstatuslabel->setText(s);
}

void set_busy_status() {
    static QString style =
        QString{"color: %1;"}.arg(themeprovider::color(THEME_FAIL).name());

    g_btnstatusicon->setStyleSheet(style);
    g_btnstatusicon->show();
}

void set_pause_status() {
    static QString style =
        QString{"color: %1;"}.arg(themeprovider::color(THEME_WARNING).name());

    g_btnstatusicon->setStyleSheet(style);
    g_btnstatusicon->show();
}

void set_ready_status() {
    static QString style =
        QString{"color: %1;"}.arg(themeprovider::color(THEME_SUCCESS).name());

    g_btnstatusicon->setStyleSheet(style);
    g_btnstatusicon->show();
}

QPushButton* status_icon() { return g_btnstatusicon; }

} // namespace statusbar
