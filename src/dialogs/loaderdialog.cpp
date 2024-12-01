#include "loaderdialog.h"
#include "../utils.h"

LoaderDialog::LoaderDialog(RDBuffer* buffer, QWidget* parent)
    : QDialog{parent}, m_ui{this} {

    m_ui.gbaddressing->setEnabled(false);

    this->populate_processors();
    usize c = rd_test(buffer, &m_testresult);

    for(usize i = 0; i < c; i++)
        m_ui.lwloaders->addItem(m_testresult[i].loader->name);

    this->populate_loglevels();

#if defined(NDEBUG)
    m_ui.cbloglevel->setCurrentIndex(0);
#else
    m_ui.cbloglevel->setCurrentIndex(3);
#endif

    connect(m_ui.lwloaders, &QListWidget::currentRowChanged, this,
            &LoaderDialog::on_loader_changed);

    // Trigger "on_loader_changed"
    if(c > 0)
        m_ui.lwloaders->setCurrentRow(0);
}

void LoaderDialog::on_loader_changed(int currentrow) {
    if(currentrow != -1)
        this->select_processor(m_testresult[currentrow].processor);
}

void LoaderDialog::accept() {
    const RDTestResult& tr = m_testresult[m_ui.lwloaders->currentRow()];
    const RDProcessor* p = tr.processor;
    this->context = tr.context;
    rd_select(tr.context);

    if(usize idx = m_ui.cbprocessors->currentIndex(); idx < m_nprocessors) {
        rd_setprocessor(m_processors[idx].id);
        p = &m_processors[idx];
    }

    utils::log(QString{"Selected loader '%1' with '%2' processor"}
                   .arg(tr.loader->name)
                   .arg(p->name));

    auto l = static_cast<RDLogLevel>(m_ui.cbloglevel->currentData().toUInt());
    rd_setloglevel(l);

    QDialog::accept();
}

void LoaderDialog::reject() {
    rd_discard();
    QDialog::reject();
}

void LoaderDialog::select_processor(const RDProcessor* processor) {
    for(usize i = 0; i < m_nprocessors; i++) {
        if(m_processors[i].id == processor->id) {
            m_ui.cbprocessors->setCurrentIndex(i);
            return;
        }
    }

    m_ui.cbprocessors->setCurrentIndex(-1);
}

void LoaderDialog::populate_processors() {
    m_nprocessors = rd_getprocessors(&m_processors);

    for(usize i = 0; i < m_nprocessors; i++)
        m_ui.cbprocessors->addItem(m_processors[i].name);
}

void LoaderDialog::populate_loglevels() const {
    m_ui.cbloglevel->addItem("Off", LOGLEVEL_OFF);
    m_ui.cbloglevel->addItem("Trace", LOGLEVEL_TRACE);
    m_ui.cbloglevel->addItem("Debug", LOGLEVEL_DEBUG);
    m_ui.cbloglevel->addItem("Info", LOGLEVEL_INFO);
    m_ui.cbloglevel->addItem("Warning", LOGLEVEL_WARNING);
    m_ui.cbloglevel->addItem("Error", LOGLEVEL_ERROR);
    m_ui.cbloglevel->addItem("Critical", LOGLEVEL_CRITICAL);
}
