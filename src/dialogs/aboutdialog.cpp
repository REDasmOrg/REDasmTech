#include "aboutdialog.h"
#include "../themeprovider.h"
#include <redasm/redasm.h>

namespace {

constexpr int SCALE_LOGO = 128;
constexpr int SCALE_TEXT = 32;

const QString ABOUT_HTML = R"(
<b>Qt Version:</b> %1<br>
<b>Version:</b> %2<br>
<b>RDAPI Level:</b> %3
<hr>
<b>Plugin Search Paths</b><br>
%4
)";

} // namespace

AboutDialog::AboutDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    this->setWindowTitle("About REDasm");

    if(themeprovider::is_dark_theme()) {
        m_ui.lbllogo->setPixmap(
            QPixmap{":/res/logo_dark.png"}.scaledToHeight(SCALE_LOGO));
    }
    else {
        m_ui.lbllogo->setPixmap(
            QPixmap{":/res/logo.png"}.scaledToHeight(SCALE_LOGO));
    }

    m_ui.lbltitle->setText("The OpenSource Disassembler");
    m_ui.lbltitle->setStyleSheet(QString{"font-size: %1px"}.arg(SCALE_TEXT));

    QString html = ABOUT_HTML.arg(QT_VERSION_STR)
                       .arg(REDASM_VERSION_STR)
                       .arg(REDASM_API_LEVEL)
                       .arg("TODO");

    m_ui.txbabout->setHtml(html);
}
