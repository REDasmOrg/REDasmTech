#include "qtui.h"
#include "../dialogs/tabledialog.h"
#include "uioptionsmodel.h"
#include <QInputDialog>
#include <QMessageBox>
#include <limits>
#include <string>

namespace qtui {

namespace {

QString qstr_if(const char* s) { return s ? s : QString{}; }

void message(const char* title, const char* text) {
    QMessageBox msgbox;
    msgbox.setWindowTitle(qstr_if(title));
    msgbox.setText(qstr_if(text));
    msgbox.exec();
}

bool confirm(const char* title, const char* text) {
    int res = QMessageBox::question(nullptr, qstr_if(title), qstr_if(text),
                                    QMessageBox::Yes | QMessageBox::No);

    return res == QMessageBox::Yes;
}

int get_item(const char* title, const char* text, const RDUIOptions* options,
             std::size_t c) {

    int selectedidx = 0;
    QStringList items;

    for(size_t i = 0; i < c; i++) {
        items.push_back(options[i].text);
        if(options[i].selected) selectedidx = i;
    }

    bool ok = false;
    QString item = QInputDialog::getItem(nullptr, qstr_if(title), qstr_if(text),
                                         items, selectedidx, false, &ok);
    if(!ok || item.isEmpty()) return -1;
    return items.indexOf(item);
}

bool get_checked(const char* title, const char* text, RDUIOptions* options,
                 usize c) {
    auto* dlgtable = new TableDialog(qstr_if(title));
    dlgtable->set_description(qstr_if(text));
    dlgtable->set_header_visible(false);
    dlgtable->set_button_box_visible(true);
    dlgtable->set_model(new UIOptionsModel(options, c));
    return dlgtable->exec() == TableDialog::Accepted;
}

const char* get_text(const char* title, const char* text, bool* ok) {
    static std::string res;
    bool isok = false;

    QString s = QInputDialog::getText(nullptr, qstr_if(title), qstr_if(text),
                                      QLineEdit::Normal, QString{}, &isok);

    if(isok)
        res = s.toStdString();
    else
        res.clear();

    if(ok) *ok = isok;
    return res.c_str();
}

double get_double(const char* title, const char* text, bool* ok) {
    return QInputDialog::getDouble(nullptr, qstr_if(title), qstr_if(text), 0,
                                   std::numeric_limits<double>::min(),
                                   std::numeric_limits<double>::max(), 1, ok);
}

i64 get_signed(const char* title, const char* text, bool* ok) {
    return QInputDialog::getInt(nullptr, title, text, 0,
                                std::numeric_limits<int>::min(),
                                std::numeric_limits<int>::max(), 1, ok);
}

u64 get_unsigned(const char* title, const char* text, bool* ok) {
    return QInputDialog::getInt(nullptr, title, text, 0,
                                std::numeric_limits<int>::min(),
                                std::numeric_limits<int>::max(), 1, ok);
}

} // namespace

void initialize(RDUI& rdui) {
    rdui.message = &qtui::message;
    rdui.confirm = &qtui::confirm;
    rdui.getitem = &qtui::get_item;
    rdui.getchecked = &qtui::get_checked;
    rdui.gettext = &qtui::get_text;
    rdui.getdouble = &qtui::get_double;
    rdui.getsigned = &qtui::get_signed;
    rdui.getunsigned = &qtui::get_unsigned;
}

} // namespace qtui
