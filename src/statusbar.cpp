#include "statusbar.h"
#include <redasm/redasm.h>

namespace statusbar {

namespace {

QLabel* g_lblstatus;

}

QLabel* set_status_label(QLabel* label) {
    g_lblstatus = label;
    return g_lblstatus;
};

void set_status_text(const QString& s) { g_lblstatus->setText(s); }

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

    if(loc.index.valid) {
        s += QString::fromWCharArray(L"<b>Index: </b>%1\u00A0\u00A0")
                 .arg(loc.index.value, 0, 16);
    }

    if(loc.segment) {
        s += QString::fromWCharArray(L"<b>Segment: </b>%1\u00A0\u00A0")
                 .arg(loc.segment);
    }

    g_lblstatus->setText(s);
}

} // namespace statusbar
