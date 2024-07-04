#pragma once

#include <QToolBar>
#include <QWidget>

class SplitView: public QWidget {
    Q_OBJECT

public:
    explicit SplitView(QWidget* w, QWidget* parent = nullptr);
    [[nodiscard]] QAction* action(int idx) const;
    [[nodiscard]] QWidget* split_view() const;
    QAction* add_button(const QIcon& icon);

private:
    [[nodiscard]] virtual SplitView* create_split() const = 0;
    void create_default_buttons();

private Q_SLOTS:
    void split_horizontal();
    void split_vertical();
    void split_in_dialog();

private:
    QAction* m_actfirstdefault{nullptr};
    QWidget* m_splitwidget{nullptr};
    QToolBar* m_tbactions;
};
