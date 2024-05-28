#pragma once

#include <QLabel>
#include <QString>
#include <redasm/redasm.h>

namespace statusbar {

// Widgets
QLabel* set_status_label(QLabel* label);

// Functions
void set_status_text(const QString& s);
void set_location(const RDSurface* surface);

} // namespace statusbar
