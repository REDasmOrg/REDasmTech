#pragma once

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <redasm/redasm.h>

namespace statusbar {

// Widgets
QLabel* set_status_label(QLabel* label);
QPushButton* set_status_icon(QPushButton* icon, int size);
QPushButton* status_icon();

// Functions
void set_status_text(const QString& s);
void set_location(const RDSurface* surface);
void set_busy_status();
void set_pause_status();
void set_ready_status();

} // namespace statusbar
