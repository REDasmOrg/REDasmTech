#pragma once

#include <QString>
#include <cstddef>
#include <redasm/redasm.h>

class MainWindow;
class QTextDocument;
class QKeyEvent;
class QWidget;
class QMenu;

namespace utils {

inline MainWindow* mainwindow{nullptr};
inline size_t bits{};

QString to_hex(size_t n);
float cell_width();
float cell_height();
void draw_surface(RDSurface* s, QTextDocument* doc, usize start, usize n);
QMenu* create_surface_menu(RDSurface* s, QWidget* w);
bool handle_key_press(RDSurface* s, QKeyEvent* e);

} // namespace utils
