#include "mainwindow.h"
#include "themeprovider.h"
#include <QApplication>
#include <QStyleFactory>

const QString REDASM_VERSION = "4.0";

int main(int argc, char** argv) {
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication app{argc, argv};
    app.setApplicationDisplayName("REDasm " + REDASM_VERSION);

    themeprovider::apply_theme();

    MainWindow mw;
    mw.show();

    return app.exec();
}
