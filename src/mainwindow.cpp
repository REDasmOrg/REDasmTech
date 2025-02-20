#include "mainwindow.h"
#include "actions.h"
#include "dialogs/analyzerdialog.h"
#include "dialogs/errordialog.h"
#include "dialogs/flcdialog.h"
#include "dialogs/loaderdialog.h"
#include "dialogs/memorymapdialog.h"
#include "dialogs/regionsdialog.h"
#include "dialogs/tabledialog.h"
#include "models/exportsmodel.h"
#include "models/importsmodel.h"
#include "models/problemsmodel.h"
#include "models/segmentsmodel.h"
#include "models/symbolsfiltermodel.h"
#include "models/symbolsmodel.h"
#include "rdui/qtui.h"
#include "settings.h"
#include "statusbar.h"
#include "themeprovider.h"
#include "utils.h"
#include "views/contextview.h"
#include "views/welcomeview.h"
#include <QDragMoveEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QStandardPaths>

namespace {

const QString PLUGINS_FOLDER_NAME = "plugins";
const QString LISTING_MODE_TEXT = "Listing";
const QString RDIL_MODE_TEXT = "RDIL";

} // namespace

MainWindow::MainWindow(QWidget* parent): QMainWindow{parent}, m_ui{this} {
    utils::mainwindow = this;

    if(themeprovider::is_dark_theme())
        this->setWindowIcon(QIcon{":/res/logo_dark.png"});
    else
        this->setWindowIcon(QIcon{":/res/logo.png"});

    m_pbrdilswitch = new QPushButton(LISTING_MODE_TEXT);
    m_pbrdilswitch->setFlat(true);
    m_pbrdilswitch->setVisible(false);
    m_pbrdilswitch->setFixedHeight(m_ui.statusbar->height());

    this->load_window_state();
    this->load_recents();
    this->update_menubar();

    m_ui.statusbar->addPermanentWidget(
        statusbar::set_status_label(new QLabel(this)), 70);

    m_ui.statusbar->addPermanentWidget(m_pbrdilswitch);

    m_ui.statusbar->addPermanentWidget(statusbar::set_status_icon(
        new QPushButton(this), m_ui.statusbar->height()));

    this->show_welcome_view();
    this->init_searchpaths();

    RDInitParams params{};
    params.onlog = [](const char* arg, void* userdata) { // onlog
        auto* self = reinterpret_cast<MainWindow*>(userdata);
        self->m_ui.logview->log(arg);
    };

    params.onstatus = [](const char* arg, void*) {
        statusbar::set_status_text(arg);
    };
    params.onerror = [](const char* arg, void*) {
        ErrorDialog::show_error("An error occurred", arg);
    };

    params.userdata = this;
    qtui::initialize(params.ui);
    rd_init(&params);

    connect(m_ui.actfileexit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui.actfileopen, &QAction::triggered, this,
            &MainWindow::select_file);
    connect(m_ui.actfileclose, &QAction::triggered, this,
            &MainWindow::show_welcome_view);
    connect(m_ui.actviewsegments, &QAction::triggered, this,
            &MainWindow::show_segments);
    connect(m_ui.actviewregions, &QAction::triggered, this,
            &MainWindow::show_regions);
    connect(m_ui.actviewstrings, &QAction::triggered, this,
            &MainWindow::show_strings);
    connect(m_ui.actviewexports, &QAction::triggered, this,
            &MainWindow::show_exports);
    connect(m_ui.actviewimports, &QAction::triggered, this,
            &MainWindow::show_imports);

    connect(m_ui.actviewmemorymap, &QAction::triggered, this, [&]() {
        auto* dlgmemorymap = new MemoryMapDialog(this);
        dlgmemorymap->show();
    });

    connect(m_ui.acttoolsflc, &QAction::triggered, this, [&]() {
        auto* dlgflc = new FLCDialog(this);
        dlgflc->show();
    });

    connect(m_ui.acttoolsproblems, &QAction::triggered, this, [&]() {
        auto* dlg = new TableDialog("Problems", this);

        QObject::connect(dlg, &TableDialog::double_clicked, this,
                         [&, dlg](const QModelIndex& index) {
                             if(ContextView* cv = this->context_view(); cv) {
                                 auto* m =
                                     static_cast<ProblemsModel*>(dlg->model());
                                 cv->jump_to(m->address(index));
                                 dlg->accept();
                             }
                         });

        dlg->set_model(new ProblemsModel(dlg));
        dlg->show();
    });

    connect(m_ui.actwinrestoredefault, &QAction::triggered, this,
            [&]() { REDasmSettings{}.restore_state(this); });

    connect(m_pbrdilswitch, &QPushButton::clicked, this,
            &MainWindow::toggle_rdil);
}

MainWindow::~MainWindow() { rd_deinit(); }

void MainWindow::dragEnterEvent(QDragEnterEvent* e) {
    if(!e->mimeData()->hasUrls()) return;
    e->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* e) {
    if(!e->mimeData()->hasUrls()) return;
    e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* e) {
    const QMimeData* mimedata = e->mimeData();
    if(!mimedata->hasUrls()) return;

    QList<QUrl> urllist = mimedata->urls();
    QString filepath = urllist.first().toLocalFile();

    QFileInfo fi(filepath);
    if(!fi.isFile()) return;

    this->open_file(filepath);
    e->acceptProposedAction();
}

void MainWindow::load_window_state() {
    if(REDasmSettings{}.restore_state(this)) return;

    QRect position = this->frameGeometry();
    position.moveCenter(qApp->primaryScreen()->availableGeometry().center());
    this->move(position.topLeft());
}

void MainWindow::load_recents() {
    m_ui.mnurecents->clear();

    REDasmSettings settings;
    QStringList recents = settings.recent_files();
    m_ui.mnurecents->setEnabled(!recents.empty());

    for(int i = 0; i < REDasmSettings::MAX_RECENT_FILES; i++) {
        if(i >= recents.length()) {
            QAction* action = m_ui.mnurecents->addAction(QString{});
            action->setVisible(false);
            continue;
        }

        if(!QFileInfo().exists(recents[i])) continue;

        QAction* action = m_ui.mnurecents->addAction(
            QString("%1 - %2").arg(i).arg(recents[i]));
        action->setData(recents[i]);

        connect(action, &QAction::triggered, this,
                [=]() { this->open_file(action->data().toString()); });
    }

    if(recents.empty()) return;

    m_ui.mnurecents->addSeparator();
    QAction* action = m_ui.mnurecents->addAction(tr("Clear"));

    connect(action, &QAction::triggered, this,
            [=]() { this->clear_recents(); });
}

void MainWindow::replace_view(QWidget* w) { // NOLINT
    w->setParent(m_ui.stackwidget);
    QWidget* oldwidget = m_ui.stackwidget->currentWidget();
    m_ui.stackwidget->addWidget(w);

    if(oldwidget) {
        m_ui.stackwidget->removeWidget(oldwidget);
        oldwidget->deleteLater();
    }
}

void MainWindow::show_welcome_view() { // NOLINT
    m_filepath.clear();
    this->setWindowTitle({});

    auto* welcomeview = new WelcomeView(m_ui.stackwidget);

    connect(welcomeview, &WelcomeView::open_requested, this,
            &MainWindow::select_file);
    connect(welcomeview, &WelcomeView::file_selected, this,
            [&](const QString& filepath) { this->open_file(filepath); });

    this->replace_view(welcomeview);
    this->enable_context_actions(false);
}

void MainWindow::show_context_view() {
    m_busy = true;

    QFileInfo fi{m_filepath};
    QDir::setCurrent(fi.path());
    this->setWindowTitle(fi.fileName());

    this->replace_view(new ContextView());
    this->enable_context_actions(true);
}

void MainWindow::update_menubar() {
    QAction* actcopy = actions::get(actions::COPY);

    m_ui.mnuedit->addAction(actcopy);

    connect(m_ui.mnuedit, &QMenu::aboutToShow, this, [&, actcopy]() {
        actcopy->setVisible(
            rdsurface_hasselection(this->context_view()->handle()));
    });
}

void MainWindow::clear_recents() {
    REDasmSettings settings;
    settings.clear_recent_files();
    this->load_recents();

    if(!this->context_view()) this->show_welcome_view(); // Recreate Welcome Tab
}

[[nodiscard]] ContextView* MainWindow::context_view() const {
    return qobject_cast<ContextView*>(m_ui.stackwidget->currentWidget());
}

bool MainWindow::loop() {
    ContextView* cv = this->context_view();

    if(m_busy && cv) {
        m_busy = rd_tick(&m_status);
        cv->tick(m_status);

        if(!m_status->busy)
            statusbar::set_ready_status();
        else if(cv->active()) {
            statusbar::set_busy_status();
            this->report_status();
        }
        else
            statusbar::set_pause_status();
    }

    return m_busy;
}

bool MainWindow::can_close() const {
    if(this->context_view()) {
        QMessageBox msgbox{const_cast<MainWindow*>(this)};
        msgbox.setWindowTitle(tr("Closing"));
        msgbox.setText(tr("Are you sure?"));
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No |
                                  QMessageBox::Cancel);

        if(msgbox.exec() != QMessageBox::Yes) return false;
    }

    return true;
}

void MainWindow::enable_context_actions(bool e) { // NOLINT
    m_ui.actfileclose->setVisible(e);
    m_ui.actedit->setVisible(e);
    m_ui.actview->setVisible(e);
    m_ui.acttools->setVisible(e);
    m_ui.acttoolsflc->setVisible(e);
    m_ui.acttoolsproblems->setVisible(e);

    m_ui.acttbseparator->setVisible(e);
    m_ui.actviewexports->setVisible(e);
    m_ui.actviewimports->setVisible(e);
    m_ui.actviewstrings->setVisible(e);

    m_pbrdilswitch->setVisible(e);
    m_ui.statusbar->setVisible(e);

    if(!e) m_ui.logview->clear();
}

void MainWindow::open_file(const QString& filepath) {
    if(filepath.isEmpty()) return;

    m_filepath = filepath;

    RDBuffer* buffer = rdbuffer_createfile(qUtf8Printable(m_filepath));
    if(!buffer) return;

    REDasmSettings settings;
    settings.update_recent_files(m_filepath);
    this->load_recents();

    auto* dlgloader = new LoaderDialog(buffer, this);

    connect(dlgloader, &LoaderDialog::accepted, this, [&, dlgloader]() {
        if(dlgloader->selected)
            this->select_analyzers();
        else
            QMessageBox::information(this, "Loader",
                                     "Loading failed or aborted");
    });

    connect(dlgloader, &LoaderDialog::rejected, this,
            [buffer]() { rdbuffer_destroy(buffer); });

    dlgloader->open();
}

void MainWindow::toggle_rdil() {
    ContextView* ctxview = this->context_view();
    if(!ctxview) return;

    bool rdil = !ctxview->has_rdil();
    ctxview->set_rdil(rdil);
    m_pbrdilswitch->setText(rdil ? RDIL_MODE_TEXT : LISTING_MODE_TEXT);
}

void MainWindow::select_file() {
    QString s = QFileDialog::getOpenFileName(this, "Disassemble file...");
    if(!s.isEmpty()) this->open_file(s);
}

void MainWindow::init_searchpaths() {
    QString searchpath = qApp->applicationDirPath() + "/lib/plugins/src";
    rd_addsearchpath(qUtf8Printable(searchpath));

    const char* appdir = std::getenv("APPDIR");
    bool isappimage = appdir && std::getenv("APPIMAGE");

    for(const QString& searchpath :
        QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
        rd_addsearchpath(qUtf8Printable(
            QDir{searchpath}.absoluteFilePath(PLUGINS_FOLDER_NAME)));

        if(!isappimage) continue;

        QString appdirqt = QString::fromUtf8(appdir);

        rd_addsearchpath(qUtf8Printable(
            QDir{appdirqt + QDir::separator() + searchpath}.absoluteFilePath(
                PLUGINS_FOLDER_NAME)));
    }
}

void MainWindow::report_status() {
    QString s;

    s += QString::fromWCharArray(L"<b>State: </b>%1\u00A0\u00A0")
             .arg(m_status->currentstep);

    if(m_status->address.valid) {
        s += QString::fromWCharArray(L"<b>Address: </b>%1\u00A0\u00A0")
                 .arg(m_status->address.value, m_status->segment->bits / 4, 16,
                      QLatin1Char('0'));
    }

    statusbar::set_status_text(s);
}

void MainWindow::select_analyzers() {
    if(rd_getanalyzers(nullptr)) {
        auto* dlganalyzers = new AnalyzerDialog(this);
        connect(dlganalyzers, &AnalyzerDialog::accepted, this,
                [&]() { this->show_context_view(); });
        connect(dlganalyzers, &AnalyzerDialog::rejected, this,
                [&]() { rdcontext_destroy(); });
        dlganalyzers->show();
    }
    else
        this->show_context_view();
}

void MainWindow::show_segments() {
    auto* dlg = new TableDialog("Segments", this);

    connect(dlg, &TableDialog::double_clicked, this,
            [&, dlg](const QModelIndex& index) {
                ContextView* ctxview = this->context_view();
                if(!ctxview) return;

                auto* segmentsmodel = static_cast<SegmentsModel*>(dlg->model());
                ctxview->jump_to(segmentsmodel->address(index));
                dlg->accept();
            });

    dlg->set_model(new SegmentsModel(dlg));
    dlg->resize_column(0, QHeaderView::Stretch);
    dlg->show();
}

void MainWindow::show_regions() {
    auto* dlgregions = new RegionsDialog(this);
    dlgregions->show();
}

void MainWindow::show_strings() {
    auto* dlg = new TableDialog("Strings", this);

    connect(dlg, &TableDialog::double_clicked, this,
            [&, dlg](const QModelIndex& index) {
                ContextView* ctxview = this->context_view();
                if(!ctxview) return;

                auto* m = static_cast<SymbolsModel*>(dlg->model());
                ctxview->jump_to(m->address(index));
                dlg->accept();
            });

    dlg->hide_column(1);
    dlg->set_model(new SymbolsFilterModel(SYMBOL_STRING, true, dlg));
    dlg->show();
}

void MainWindow::show_exports() {
    auto* dlg = new TableDialog("Exports", this);

    connect(dlg, &TableDialog::double_clicked, this,
            [&, dlg](const QModelIndex& index) {
                ContextView* ctxview = this->context_view();
                if(!ctxview) return;

                auto* m = static_cast<ExportsModel*>(dlg->model());
                ctxview->jump_to(m->address(index));
                dlg->accept();
            });

    dlg->set_model(new ExportsModel(dlg));
    dlg->show();
}

void MainWindow::show_imports() {
    auto* dlg = new TableDialog("Imports", this);

    connect(dlg, &TableDialog::double_clicked, this,
            [&, dlg](const QModelIndex& index) {
                ContextView* ctxview = this->context_view();
                if(!ctxview) return;

                auto* m = static_cast<ImportsModel*>(dlg->model());
                ctxview->jump_to(m->address(index));
                dlg->accept();
            });

    dlg->set_model(new ImportsModel(dlg));
    dlg->show();
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if(this->can_close()) {
        REDasmSettings{}.save_state(this);
        QWidget::closeEvent(e);
        Q_EMIT closed();
    }
    else
        e->ignore();
}
