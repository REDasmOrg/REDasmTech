#include "splitview.h"
#include "../fontawesome.h"
#include <QVBoxLayout>

SplitView::SplitView(QWidget* w, QWidget* parent)
    : QWidget{parent}, m_splitwidget{w} {
    m_tbactions = new QToolBar();
    m_tbactions->setIconSize({16, 16});
    m_tbactions->setToolButtonStyle(Qt::ToolButtonIconOnly);

    this->create_default_buttons();

    auto* vbox = new QVBoxLayout(this);
    vbox->setSpacing(0);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(m_tbactions);
    vbox->addWidget(w);
}

QAction* SplitView::action(int idx) const {
    auto actions = m_tbactions->actions();
    return actions.at(idx);
}

QWidget* SplitView::split_view() const { return m_splitwidget; }

QAction* SplitView::add_button(const QIcon& icon) {
    auto* act = new QAction(icon, QString{}, m_tbactions);
    m_tbactions->insertAction(m_actfirstdefault, act);
    return act;
}

void SplitView::create_default_buttons() {
    auto* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_actfirstdefault = m_tbactions->addWidget(empty);

    connect(m_tbactions->addAction(FA_ICON(0xf105), QString()),
            &QAction::triggered, this, &SplitView::split_horizontal);
    connect(m_tbactions->addAction(FA_ICON(0xf107), QString()),
            &QAction::triggered, this, &SplitView::split_vertical);
    connect(m_tbactions->addAction(FA_ICON(0xf2d2), QString()),
            &QAction::triggered, this, &SplitView::split_in_dialog);
}

void SplitView::split_horizontal() {}

void SplitView::split_vertical() {}

void SplitView::split_in_dialog() {}
