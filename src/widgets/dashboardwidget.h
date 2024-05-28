#pragma once

#include <QWidget>

class QPushButton;
class QLabel;

class DashboardWidget: public QWidget {
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget* parent = nullptr);

protected:
    void make_bordered(QPushButton* pb) const;
    void apply_logo(QLabel* lbl) const;
};
