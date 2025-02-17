#pragma once

#include <QString>
#include <redasm/redasm.h>

class MainWindow;
class QTextDocument;
class QKeyEvent;
class QWidget;
class QMenu;

namespace utils {

inline MainWindow* mainwindow{nullptr};

inline void log(const QString& s) { rd_log(qUtf8Printable(s)); }

float cell_width();
float cell_height();
QString to_hex_addr(RDAddress address, const RDSegment* seg = nullptr);
void show_detail(const QString& html, QWidget* parent = nullptr);
usize draw_surface(RDSurface* s, QTextDocument* doc, usize start, usize n);
QMenu* create_surface_menu(RDSurface* s, QWidget* w);
bool handle_key_press(RDSurface* s, QKeyEvent* e);

} // namespace utils
