#pragma once

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <redasm/redasm.h>

namespace statusbar {

// Widgets
QLabel* set_status_label(QLabel* label);
QPushButton* set_status_icon(QPushButton* icon, int height);
QPushButton* status_icon();

// Functions
void set_status_text(const QString& s);
void set_location(const RDSurface* surface);
void set_play_status();
void set_pause_status();
void set_stop_status();
void hide_status_icon();

} // namespace statusbar
