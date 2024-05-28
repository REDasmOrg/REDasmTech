#include "memorymapdialog.h"
#include "../hexview/flagsbuffer.h"

MemoryMapDialog::MemoryMapDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    auto* flagsbuffer = new FlagsBuffer();
    QHexDocument* hexdocument = QHexDocument::fromBuffer(flagsbuffer);
    m_ui.hexview->setDocument(hexdocument);
    m_ui.hexview->setBaseAddress(flagsbuffer->base_address());
}
