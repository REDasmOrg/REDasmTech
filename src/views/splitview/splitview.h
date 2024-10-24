#pragma once

#include <QSplitter>

class SplitWidget;

class SplitDelegate: public QObject {
    Q_OBJECT

public:
    explicit SplitDelegate(QObject* parent = nullptr): QObject{parent} {}
    [[nodiscard]] virtual QWidget* create_widget(SplitWidget* split) = 0;
};

class SplitView: public QSplitter {
    Q_OBJECT

public:
    explicit SplitView(SplitDelegate* delegate, QWidget* parent = nullptr);
    [[nodiscard]] SplitDelegate* split_delegate() const { return m_delegate; }
    [[nodiscard]] SplitWidget* selected_split() const { return m_selsplit; }
    [[nodiscard]] int count() const { return m_count; }

private:
    void set_selected_widget(SplitWidget* sw);
    void inc_count();
    void dec_count();

Q_SIGNALS:
    void selected_changed();
    void count_changed();

private:
    int m_count{0};
    SplitDelegate* m_delegate;
    SplitWidget* m_selsplit{nullptr};

    friend class SplitWidget;
};
