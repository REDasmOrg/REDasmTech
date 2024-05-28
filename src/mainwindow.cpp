#include "mainwindow.h"
#include "actions.h"
#include "dialogs/analyzerdialog.h"
#include "dialogs/flcdialog.h"
#include "dialogs/loaderdialog.h"
#include "dialogs/memorymapdialog.h"
#include "dialogs/tabledialog.h"
#include "models/segmentsmodel.h"
#include "models/symbolsfiltermodel.h"
#include "models/symbolsmodel.h"
#include "settings.h"
#include "statusbar.h"
#include "themeprovider.h"
#include "utils.h"
#include "widgets/contextview.h"
#include "widgets/welcomeview.h"
#include <QDragMoveEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>

MainWindow::MainWindow(QWidget* parent): QMainWindow{parent}, m_ui{this} {
    utils::mainwindow = this;

    if(themeprovider::is_dark_theme())
        this->setWindowIcon(QIcon(":/res/logo_dark.png"));
    else
        this->setWindowIcon(QIcon(":/res/logo.png"));

    this->show_welcome_view();
    this->load_recents();

    actions::init(this);
    this->update_menubar();

    rd_setloglevel(LOGLEVEL_TRACE);

    QString searchpath = qApp->applicationDirPath() + "/lib/plugins/src";
    rd_addsearchpath(qUtf8Printable(searchpath));

    rd_init();

    m_ui.statusbar->addPermanentWidget(
        statusbar::set_status_label(new QLabel(this)), 70);

    connect(m_ui.actfileexit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui.actfileopen, &QAction::triggered, this,
            &MainWindow::select_file);
    connect(m_ui.actfileclose, &QAction::triggered, this,
            &MainWindow::show_welcome_view);
    connect(m_ui.actviewsegments, &QAction::triggered, this,
            &MainWindow::show_segments);
    connect(m_ui.actviewstrings, &QAction::triggered, this,
            &MainWindow::show_strings);

    connect(m_ui.actviewmemorymap, &QAction::triggered, this, [&]() {
        auto* dlgmemorymap = new MemoryMapDialog(this);
        dlgmemorymap->show();
    });

    connect(m_ui.acttoolsflc, &QAction::triggered, this, [&]() {
        auto* dlgflc = new FLCDialog(this);
        dlgflc->show();
    });
}

MainWindow::~MainWindow() { rd_deinit(); }

void MainWindow::dragEnterEvent(QDragEnterEvent* e) {
    if(!e->mimeData()->hasUrls())
        return;
    e->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* e) {
    if(!e->mimeData()->hasUrls())
        return;
    e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* e) {
    const QMimeData* mimedata = e->mimeData();
    if(!mimedata->hasUrls())
        return;

    QList<QUrl> urllist = mimedata->urls();
    QString filepath = urllist.first().toLocalFile();

    QFileInfo fi(filepath);
    if(!fi.isFile())
        return;

    this->open_file(filepath);
    e->acceptProposedAction();
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

        if(!QFileInfo().exists(recents[i]))
            continue;

        QAction* action = m_ui.mnurecents->addAction(
            QString("%1 - %2").arg(i).arg(recents[i]));
        action->setData(recents[i]);

        connect(action, &QAction::triggered, this,
                [=]() { this->open_file(action->data().toString()); });
    }

    if(recents.empty())
        return;

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

void MainWindow::show_context_view(bool candisassemble) {
    if(candisassemble)
        rd_disassemble();

    QFileInfo fi{m_filepath};
    QDir::setCurrent(fi.path());
    this->setWindowTitle(fi.fileName());

    REDasmSettings settings;
    settings.update_recent_files(m_filepath);
    this->load_recents();
    this->replace_view(new ContextView());
    this->enable_context_actions(true);
}

void MainWindow::update_menubar() {
    QAction* actcopy = actions::get(actions::COPY);

    m_ui.mnuedit->addAction(actcopy);

    connect(m_ui.mnuedit, &QMenu::aboutToShow, this, [&, actcopy]() {
        SurfaceView* sv = this->context_view()->surface_view();
        actcopy->setVisible(rdsurface_hasselection(sv->handle()));
    });
}

void MainWindow::clear_recents() {
    REDasmSettings settings;
    settings.clear_recent_files();
    this->load_recents();

    if(!this->context_view())
        this->show_welcome_view(); // Recreate Welcome Tab
}

[[nodiscard]] ContextView* MainWindow::context_view() const {
    return qobject_cast<ContextView*>(m_ui.stackwidget->currentWidget());
}

bool MainWindow::can_close() const {
    if(qobject_cast<ContextView*>(m_ui.stackwidget->currentWidget())) {
        QMessageBox msgbox{const_cast<MainWindow*>(this)};
        msgbox.setWindowTitle(tr("Closing"));
        msgbox.setText(tr("Are you sure?"));
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No |
                                  QMessageBox::Cancel);

        if(msgbox.exec() != QMessageBox::Yes)
            return false;
    }

    return true;
}

void MainWindow::enable_context_actions(bool e) { // NOLINT
    m_ui.actfileclose->setVisible(e);
    m_ui.actedit->setVisible(e);
    m_ui.actview->setVisible(e);
    m_ui.acttools->setVisible(e);
    m_ui.acttoolsflc->setVisible(e);
}

void MainWindow::open_file(const QString& filepath) {
    if(filepath.isEmpty())
        return;

    m_filepath = filepath;

    RDBuffer* buffer = rd_loadfile(qUtf8Printable(m_filepath));
    if(!buffer)
        return;

    auto* dlgloader = new LoaderDialog(buffer, this);

    connect(dlgloader, &LoaderDialog::accepted, this, [&, dlgloader]() {
        this->select_analyzers(dlgloader->can_disassemble());
    });

    dlgloader->open();
}

void MainWindow::select_file() {
    QString s = QFileDialog::getOpenFileName(this, "Disassemble file...");
    if(!s.isEmpty())
        this->open_file(s);
}

void MainWindow::select_analyzers(bool candisassemble) {
    if(rd_getanalyzers(nullptr)) {
        auto* dlganalyzers = new AnalyzerDialog(this);
        connect(
            dlganalyzers, &AnalyzerDialog::accepted, this,
            [&, candisassemble]() { this->show_context_view(candisassemble); });
        dlganalyzers->open();
    }
    else
        this->show_context_view(candisassemble);
}

void MainWindow::show_segments() {
    auto* dlg = new TableDialog("Segments", this);

    connect(dlg, &TableDialog::double_clicked, this,
            [&, dlg](const QModelIndex& index) {
                ContextView* ctxview = this->context_view();
                if(!ctxview)
                    return;

                auto* segmentsmodel = static_cast<SegmentsModel*>(dlg->model());
                ctxview->surface_view()->jump_to(segmentsmodel->address(index));
                dlg->accept();
            });

    dlg->set_model(new SegmentsModel(dlg));
    dlg->show();
}

void MainWindow::show_strings() {
    auto* dlg = new TableDialog("Strings", this);

    connect(dlg, &TableDialog::double_clicked, this,
            [&, dlg](const QModelIndex& index) {
                ContextView* ctxview = this->context_view();
                if(!ctxview)
                    return;

                auto* m = static_cast<SymbolsModel*>(dlg->model());
                ctxview->surface_view()->jump_to(m->address(index));
                dlg->accept();
            });

    dlg->hide_column(1);
    dlg->set_model(new SymbolsFilterModel(SYMBOL_STRING, dlg));
    dlg->show();
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if(this->can_close())
        QWidget::closeEvent(e);
    else
        e->ignore();
}
