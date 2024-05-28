#pragma once

#include <QString>
#include <cstddef>

class MainWindow;

namespace utils {

inline MainWindow* mainwindow{nullptr};
inline size_t bits{};

QString to_hex(size_t n);

} // namespace utils
