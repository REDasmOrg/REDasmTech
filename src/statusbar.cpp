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
                 .arg(loc.segment);
    }

    s += QString::fromWCharArray(L"<b>Index: </b>%1\u00A0\u00A0")
             .arg(loc.index, 0, 16);

    g_lblstatus->setText(s);
}

} // namespace statusbar
