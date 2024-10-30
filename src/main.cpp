#include "mainwindow.h"
#include "themeprovider.h"
#include <QApplication>
#include <QStyleFactory>

namespace {

const QString REDASM_VERSION = "4.0";
bool running = true;

} // namespace

int main(int argc, char** argv) {
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication app{argc, argv};
    app.setApplicationDisplayName("REDasm " + REDASM_VERSION);

    rd_setloglevel(LOGLEVEL_INFO);
    themeprovider::apply_theme();

    MainWindow mw;
    QObject::connect(&mw, &MainWindow::closed, []() { running = false; });
    mw.show();

    while(running) {
        QEventLoop::ProcessEventsFlags f = QEventLoop::WaitForMoreEvents;

        if(mw.loop())
            f = QEventLoop::AllEvents;

        app.processEvents(f);
    }

    return 0;
}
