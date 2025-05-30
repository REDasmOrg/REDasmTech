#include "loaderdialog.h"
#include "../utils.h"

LoaderDialog::LoaderDialog(RDBuffer* buffer, QWidget* parent)
    : QDialog{parent}, m_ui{this} {

    m_ui.gbaddressing->setEnabled(false);

    this->populate_processors();
    m_testresult = rd_test(buffer);

    for(isize i = 0; i < m_testresult->length; i++)
        m_ui.lwloaders->addItem(m_testresult->data[i].loaderplugin->name);

    this->populate_loglevels();

#if defined(NDEBUG)
    m_ui.cbloglevel->setCurrentIndex(0);
#else
    m_ui.cbloglevel->setCurrentIndex(3);
#endif

    connect(m_ui.lwloaders, &QListWidget::currentRowChanged, this,
            &LoaderDialog::on_loader_changed);

    connect(m_ui.cbprocessors, &QComboBox::currentIndexChanged, this,
            &LoaderDialog::on_processor_changed);

    // Trigger "on_loader_changed"
    if(m_testresult->length > 0) m_ui.lwloaders->setCurrentRow(0);
}

void LoaderDialog::on_loader_changed(int currentrow) {
    if(currentrow != -1)
        this->select_processor(m_testresult->data[currentrow].processorplugin);
}

void LoaderDialog::on_processor_changed(int currentrow) {
    int loaderidx = m_ui.lwloaders->currentRow();
    if(loaderidx == -1) return;

    if(currentrow != -1) {
        m_testresult->data[loaderidx].processorplugin =
            m_processors->data[currentrow];
    }
}

void LoaderDialog::accept() {
    const RDTestResult& tr = m_testresult->data[m_ui.lwloaders->currentRow()];
    this->selected = rd_select(&tr);

    if(this->selected) {
        utils::log(QString{"Loader: %1"}.arg(tr.loaderplugin->name));
        utils::log(QString{"Processor: %1"}.arg(tr.processorplugin->name));

        const RDCallingConvention* cc = rd_getcallingconvention();
        if(cc) utils::log(QString{"Calling Convention: %1"}.arg(cc->name));

        const RDEnvironment* e = rd_getenvironment();
        if(e->name) utils::log(QString{"Environment: %1"}.arg(e->name));

        auto l =
            static_cast<RDLogLevel>(m_ui.cbloglevel->currentData().toUInt());
        rd_setloglevel(l);
    }

    QDialog::accept();
}

void LoaderDialog::reject() {
    rd_discard();
    QDialog::reject();
}

void LoaderDialog::select_processor(const RDProcessorPlugin* proc) {
    for(isize i = 0; i < m_processors->length; i++) {
        if(m_processors->data[i]->id == proc->id) {
            m_ui.cbprocessors->setCurrentIndex(i);
            return;
        }
    }

    m_ui.cbprocessors->setCurrentIndex(-1);
}

void LoaderDialog::populate_processors() {
    m_processors = rd_getprocessorplugins();

    const RDProcessorPlugin** plugin;
    slice_foreach(plugin, m_processors)
        m_ui.cbprocessors->addItem((*plugin)->name);
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
