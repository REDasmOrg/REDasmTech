#pragma once

#include "../ui/welcomeview.h"
#include "dashboardview.h"

class WelcomeView: public DashboardView {
    Q_OBJECT

public:
    explicit WelcomeView(QWidget* parent = nullptr);

private Q_SLOTS:
    void on_file_selected(const QModelIndex& index);

Q_SIGNALS:
    void file_selected(const QString& filepath);
    void settings_requested();
    void about_requested();
    void open_requested();

private:
    ui::WelcomeView m_ui;
};
