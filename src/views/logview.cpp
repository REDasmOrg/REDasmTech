#include "logview.h"
#include <QVBoxLayout>

LogView::LogView(QWidget* parent): QWidget{parent} {
    this->setVisible(false);
    m_ptelogs = new QPlainTextEdit(this);
    m_ptelogs->setUndoRedoEnabled(false);
    m_ptelogs->setReadOnly(true);

    auto* vbox = new QVBoxLayout(this);
    vbox->addWidget(m_ptelogs);
    vbox->setContentsMargins(0, 0, 0, 0);
}

void LogView::log(const QString& s) {
    m_ptelogs->insertPlainText(s);
    m_ptelogs->insertPlainText("\n");
    this->setVisible(true);
}
