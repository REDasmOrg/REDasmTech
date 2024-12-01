#pragma once

#include <QAction>

class MainWindow;

namespace actions {

enum Type : quint8 {
    GOTO,
    COPY,
    REFS_TO,
    RENAME,
    COMMENT,
};

void init(MainWindow* mw);
QAction* get(Type t);

} // namespace actions
