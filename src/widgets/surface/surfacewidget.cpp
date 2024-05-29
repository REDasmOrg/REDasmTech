#include "surfacewidget.h"
#include "../../actions.h"
#include "../../statusbar.h"
#include "../../themeprovider.h"
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextDocument>

SurfaceWidget::SurfaceWidget(QWidget* parent): QAbstractScrollArea{parent} {
    m_surface = rdsurface_new();

    this->setCursor(Qt::ArrowCursor);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFont(QFont{"monospace"});
    this->setFrameShape(QFrame::NoFrame);
    this->setPalette(qApp->palette());
    this->verticalScrollBar()->setMinimum(0);
    this->verticalScrollBar()->setValue(0);
    this->verticalScrollBar()->setSingleStep(1);
    this->verticalScrollBar()->setPageStep(1);
    this->horizontalScrollBar()->setSingleStep(1);
    this->horizontalScrollBar()->setMinimum(0);
    this->horizontalScrollBar()->setValue(0);

    this->create_context_menu();

    m_document.setDefaultFont(this->font());
    m_document.setUndoRedoEnabled(false);

    QFontMetricsF fm{this->font()};
    m_cellwidth = fm.horizontalAdvance(" ");
    m_cellheight = fm.height();

    connect(this->verticalScrollBar(), &QScrollBar::actionTriggered, this,
            [&](int action) {
                switch(action) {
                    case QScrollBar::SliderSingleStepAdd: {
                        size_t idx = this->get_surface_index();

                        if(idx < this->get_listing_length()) {
                            rdsurface_seek(m_surface, idx + 1);
                            this->viewport()->update();
                        }
                        break;
                    }

                    case QScrollBar::SliderSingleStepSub: {
                        size_t idx = this->get_surface_index();

                        if(idx > 0) {
                            rdsurface_seek(m_surface, idx - 1);
                            this->viewport()->update();
                        }
                        break;
                    }

                    case QScrollBar::SliderMove: {
                        rdsurface_seek(
                            m_surface,
                            this->verticalScrollBar()->sliderPosition());

                        this->viewport()->update();
                        break;
                    }

                    default: break;
                }
            });

    this->update_scrollbars();
}

SurfaceWidget::~SurfaceWidget() {
    rd_free(m_surface);
    m_surface = nullptr;
}

void SurfaceWidget::jump_to(RDAddress address) {
    usize idx;

    if(rdlisting_getindex(address, &idx)) {
        const size_t DIFF = (this->visible_lines() / 4);
        size_t relidx = idx;
        if(relidx > DIFF)
            relidx -= DIFF;

        rdsurface_setposition(m_surface, idx - relidx, 0);
        this->verticalScrollBar()->setValue(relidx);
    }
}

void SurfaceWidget::mouseDoubleClickEvent(QMouseEvent* e) {
    if(e->button() == Qt::LeftButton) {
        if(!this->follow_under_cursor()) {
            RDSurfacePosition p = this->get_surface_coords(e->position());
            if(rdsurface_selectword(m_surface, p.row, p.col))
                this->viewport()->update();
        }
    }

    QAbstractScrollArea::mouseDoubleClickEvent(e);
}

void SurfaceWidget::mousePressEvent(QMouseEvent* e) {
    if(m_surface) {
        if(e->button() == Qt::LeftButton) {
            RDSurfacePosition p = this->get_surface_coords(e->position());
            rdsurface_setposition(m_surface, p.row, p.col);
            this->viewport()->update();
        }
        // else if(event->button() == Qt::BackButton)
        // m_surface->goBack();
        // else if(event->button() == Qt::ForwardButton)
        // m_surface->goForward();
        else
            return;

        e->accept();
    }

    QAbstractScrollArea::mousePressEvent(e);
}

void SurfaceWidget::mouseMoveEvent(QMouseEvent* e) {
    if(m_surface && (e->buttons() == Qt::LeftButton)) {
        RDSurfacePosition p = this->get_surface_coords(e->position());
        if(rdsurface_select(m_surface, p.row, p.col))
            this->viewport()->update();
        e->accept();
        return;
    }

    QAbstractScrollArea::mouseMoveEvent(e);
}

void SurfaceWidget::resizeEvent(QResizeEvent* e) {
    this->update_scrollbars();
    QAbstractScrollArea::resizeEvent(e);

    rdsurface_setcolumns(m_surface, this->visible_columns());
    this->viewport()->update();
}

void SurfaceWidget::keyPressEvent(QKeyEvent* e) {
    QScrollBar* vscroll = this->verticalScrollBar();
    auto [row, col] = this->get_surface_position();

    if(e->matches(QKeySequence::MoveToNextChar)) {
        rdsurface_setposition(m_surface, row, col + 1);
    }
    else if(e->matches(QKeySequence::MoveToPreviousChar)) {
        if(col == 0)
            return;
        rdsurface_setposition(m_surface, row, col - 1);
    }
    else if(e->matches(QKeySequence::MoveToNextLine)) {
        rdsurface_setposition(m_surface, row + 1, col);
    }
    else if(e->matches(QKeySequence::MoveToPreviousLine)) {
        if(row == 0)
            return;
        rdsurface_setposition(m_surface, row - 1, col);
    }
    else if(e->matches(QKeySequence::MoveToStartOfLine)) {
        rdsurface_setposition(m_surface, row, 0);
    }
    else if(e->matches(QKeySequence::MoveToEndOfLine)) {
        rdsurface_setposition(m_surface, row, this->visible_columns());
    }
    else if(e->matches(QKeySequence::MoveToStartOfDocument)) {
        vscroll->setValue(0);
        return;
    }
    else if(e->matches(QKeySequence::MoveToEndOfDocument)) {
        vscroll->setValue(vscroll->value() + this->visible_lines() - 1);
        return;
    }
    else if(e->matches(QKeySequence::SelectNextChar)) {
        rdsurface_select(m_surface, row, col + 1);
    }
    else if(e->matches(QKeySequence::SelectPreviousChar)) {
        if(col == 0)
            return;
        rdsurface_select(m_surface, row, col - 1);
    }
    else if(e->matches(QKeySequence::SelectNextLine)) {
        rdsurface_select(m_surface, row + 1, col);
    }
    else if(e->matches(QKeySequence::SelectPreviousLine)) {
        if(row == 0)
            return;
        rdsurface_select(m_surface, row - 1, col);
    }
    else if(e->matches(QKeySequence::SelectStartOfLine)) {
        rdsurface_select(m_surface, row, 0);
    }
    else if(e->matches(QKeySequence::SelectEndOfLine)) {
        rdsurface_select(m_surface, row, this->visible_columns());
    }
    else if(e->matches(QKeySequence::SelectStartOfDocument)) {
        rdsurface_select(m_surface, 0, 0);
    }
    else if(e->matches(QKeySequence::SelectEndOfDocument)) {
        rdsurface_select(m_surface, this->visible_lines() - row,
                         this->visible_columns());
    }
    else if(e->matches(QKeySequence::SelectAll)) {
        rdsurface_setposition(m_surface, 0, 0);
        rdsurface_select(m_surface, this->visible_lines(),
                         this->visible_columns());
    }
    else {
        QAbstractScrollArea::keyPressEvent(e);
        return;
    }

    this->viewport()->update();
}

void SurfaceWidget::paintEvent(QPaintEvent* e) {
    if(!m_surface) {
        QAbstractScrollArea::paintEvent(e);
        return;
    }

    rdsurface_render(m_surface, this->verticalScrollBar()->value(),
                     this->visible_lines());

    m_document.clear();
    QTextCursor cursor(&m_document);
    cursor.beginEditBlock();

    usize nrows = rdsurface_getrowcount(m_surface);

    for(usize i = 0; i < nrows; i++) {
        const RDSurfaceCell* row = nullptr;
        usize ncols = rdsurface_getrow(m_surface, i, &row);

        for(usize j = 0; j < ncols; j++) {
            QTextCharFormat cf;
            QColor fg = themeprovider::color(row[j].fg);
            QColor bg = themeprovider::color(row[j].bg);

            if(fg.isValid())
                cf.setForeground(fg);
            if(bg.isValid())
                cf.setBackground(bg);

            cursor.insertText(QChar{row[j].ch}, cf);
        }

        cursor.insertBlock();
    }

    cursor.endEditBlock();

    QPainter painter{this->viewport()};
    painter.fillRect(this->rect(), Qt::white);
    m_document.drawContents(&painter);

    statusbar::set_location(m_surface);
}

int SurfaceWidget::visible_columns() const {
    return qFloor(this->viewport()->width() / m_cellwidth);
}

int SurfaceWidget::visible_lines() const {
    return qCeil(this->viewport()->height() / m_cellheight);
}

RDSurfacePosition SurfaceWidget::get_surface_coords(QPointF pt) const {
    auto* layout = m_document.documentLayout();
    int pos = layout->hitTest(pt, Qt::FuzzyHit);

    if(pos == -1)
        qFatal("Invalid position");

    QTextCursor c(const_cast<QTextDocument*>(&m_document));
    c.setPosition(pos);

    return {
        static_cast<usize>(c.blockNumber()),
        static_cast<usize>(c.columnNumber()),
    };
}

RDSurfacePosition SurfaceWidget::get_surface_position() const {
    RDSurfacePosition pos;
    rdsurface_getposition(m_surface, &pos);
    return pos;
}

size_t SurfaceWidget::get_surface_index() const {
    return rdsurface_getindex(m_surface);
}

size_t SurfaceWidget::get_listing_length() const {
    return rdlisting_getlength();
}

bool SurfaceWidget::follow_under_cursor() {
    RDAddress address;

    if(rdsurface_getaddressundercursor(m_surface, &address)) {
        this->jump_to(address);
        return true;
    }

    return false;
}

void SurfaceWidget::create_context_menu() {
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    QAction* actcopy = actions::get(actions::COPY);

    m_menu = new QMenu(this);
    m_menu->addAction(actions::get(actions::GOTO));
    m_menu->addSeparator();
    m_menu->addAction(actcopy);

    connect(m_menu, &QMenu::aboutToShow, this, [&, actcopy]() {
        actcopy->setVisible(rdsurface_hasselection(m_surface));
    });

    connect(this, &SurfaceWidget::customContextMenuRequested, this,
            [&](const QPoint&) { m_menu->popup(QCursor::pos()); });
}

void SurfaceWidget::update_scrollbars() {
    this->verticalScrollBar()->setRange(0, this->get_listing_length());
}
