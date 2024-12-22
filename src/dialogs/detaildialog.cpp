#include "detaildialog.h"
#include "../settings.h"

DetailDialog::DetailDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    m_ui.tbdetail->setFont(REDasmSettings::font());
}

void DetailDialog::set_html(const QString& s) { // NOLINT
    m_ui.tbdetail->setHtml(s);
}
