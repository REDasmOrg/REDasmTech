#pragma once

#include <QAction>

class MainWindow;

namespace actions {

enum Type {
    GOTO,
    COPY,
};

void init(MainWindow* mw);
QAction* get(Type t);

} // namespace actions
