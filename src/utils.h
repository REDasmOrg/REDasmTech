#pragma once

#include <QString>
#include <cstddef>
#include <redasm/redasm.h>

class MainWindow;
class QTextDocument;

namespace utils {

inline MainWindow* mainwindow{nullptr};
inline size_t bits{};

QString to_hex(size_t n);
float cell_width();
float cell_height();
void draw_surface(RDSurface* s, QTextDocument* doc, usize start, usize n);

} // namespace utils
