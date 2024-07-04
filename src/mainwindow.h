#pragma once

#include "ui/mainwindow.h"
#include "views/contextview.h"

class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
    [[nodiscard]] ContextView* context_view() const;
    bool loop();

protected:
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* e) override;
    void closeEvent(QCloseEvent* e) override;

private Q_SLOTS:
    void select_file();
    void show_segments();
    void show_strings();
    void show_exports();
    void show_imports();
    void show_welcome_view();

private:
    [[nodiscard]] bool can_close() const;
    void report_status();
    void select_analyzers();
    void show_context_view();
    void enable_context_actions(bool e);
    void open_file(const QString& filepath);
    void replace_view(QWidget* w);
    void update_menubar();
    void clear_recents();
    void load_recents();

Q_SIGNALS:
    void closed();

private:
    const RDEngineStatus* m_status{nullptr};
    ui::MainWindow m_ui;
    QString m_filepath;
    bool m_busy{false};
};
