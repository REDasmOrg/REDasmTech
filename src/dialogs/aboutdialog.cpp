#include "aboutdialog.h"
#include "../themeprovider.h"
#include <redasm/redasm.h>

namespace {

constexpr int SCALE_LOGO = 128;
constexpr int SCALE_TEXT = 32;

const QString ABOUT_HTML = R"(
<div><b>Qt Version:</b> %1</div>
<div><b>Version:</b> %2</div>
<div><b>RDAPI Level:</b> %3</div>
<hr>
<div><b>Search Paths</b></div>
<div>%4</div>
<hr>
<table>
    <tr>
        <th align="center" valign="middle" width="50%">Loaders</th>
        <th align="center" valign="middle" width="50%">Processors</th>
    </tr>
    %5
</table>
)";

void compile_versions(QString& html) {
    html =
        html.arg(QT_VERSION_STR).arg(REDASM_VERSION_STR).arg(REDASM_API_LEVEL);
}

void compile_searchpaths(QString& html) {
    const char** searchpaths = nullptr;
    usize c = rd_getsearchpaths(&searchpaths);

    if(c > 0) {
        QString lines;

        for(usize i = 0; i < c; i++)
            lines.append(QString{"<div>- %1</div>"}.arg(searchpaths[i]));

        html = html.arg(lines);
    }
    else {
        // clang-format off
        html = html.arg(QString{R"(
            <font color="%1">
                <b>No Search paths set</b>
            </font>
        )"}.arg(themeprovider::color(THEME_FAIL).name()));
        // clang-format on
    }
}

void compile_plugins(QString& html) {
    const RDLoaderPluginSlice* loaders = rd_getloaderplugins();
    const RDProcessorPluginSlice* processors = rd_getprocessorplugins();
    QString plugins;

    for(isize i = 0; i < qMax(loaders->length, processors->length); i++) {
        QString row = R"(
            <tr>
                <td align="center" valign="middle">%1</td>
                <td align="center" valign="middle">%2</td>
            </tr>
        )";

        if(i < loaders->length)
            row = row.arg(QString{"%1 (%2)"}
                              .arg(loaders->data[i]->name)
                              .arg(loaders->data[i]->id));
        else
            row = row.arg(QString{});

        if(i < processors->length) {
            row = row.arg(QString{"%1 (%2)"}
                              .arg(processors->data[i]->name)
                              .arg(processors->data[i]->id));
        }
        else
            row = row.arg(QString{});

        plugins.append(row);
    }

    html = html.arg(plugins);
}

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

    QString html = ABOUT_HTML;
    compile_versions(html);
    compile_searchpaths(html);
    compile_plugins(html);
    m_ui.txbabout->setHtml(html);
}
