#pragma once

#include <QPlainTextEdit>
#include <QWidget>

class LogView: public QWidget {
    Q_OBJECT

public:
    explicit LogView(QWidget* parent = nullptr);

public Q_SLOTS:
    void log(const QString& s);

private:
    QPlainTextEdit* m_ptelogs;
};
