#include "surfacewidget.h"
#include "../../settings.h"
#include "../../statusbar.h"
#include "../../utils.h"
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

static constexpr int SCROLL_SPEED = 3;

SurfaceWidget::SurfaceWidget(QWidget* parent): QAbstractScrollArea{parent} {
    m_surface = rdsurface_create(SURFACE_DEFAULT);
    m_popup = new SurfacePopup(this);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setCursor(Qt::ArrowCursor);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFrameShape(QFrame::NoFrame);
    this->setFont(REDasmSettings::font());
    this->setPalette(qApp->palette());
    this->verticalScrollBar()->setMinimum(0);
    this->verticalScrollBar()->setValue(0);
    this->verticalScrollBar()->setSingleStep(SCROLL_SPEED);
    this->verticalScrollBar()->setPageStep(1);
    this->horizontalScrollBar()->setSingleStep(1);
    this->horizontalScrollBar()->setMinimum(0);
    this->horizontalScrollBar()->setValue(0);

    m_menu = utils::create_surface_menu(m_surface, this);

    connect(this, &SurfaceWidget::customContextMenuRequested, this,
            [&](const QPoint&) { m_menu->popup(QCursor::pos()); });

    m_document.setDefaultFont(this->font());
    m_document.setUndoRedoEnabled(false);
    m_document.setDocumentMargin(0);

    connect(this->verticalScrollBar(), &QScrollBar::actionTriggered, this,
            [&](int action) {
                switch(action) {
                    case QScrollBar::SliderSingleStepAdd: {
                        auto lidx = this->get_listing_index();

                        if(lidx && *lidx < this->get_listing_length()) {
                            rdsurface_clearselection(m_surface);
                            rdsurface_seekposition(m_surface,
                                                   *lidx + 1); // SCROLL_SPEED);
                            this->viewport()->update();
                        }
                        else
                            return;
                        break;
                    }

                    case QScrollBar::SliderSingleStepSub: {
                        auto lidx = this->get_listing_index();

                        if(lidx && *lidx > 0) {
                            rdsurface_clearselection(m_surface);
                            rdsurface_seekposition(m_surface,
                                                   *lidx - 1); // SCROLL_SPEED);
                            this->viewport()->update();
                        }
                        else
                            return;
                        break;
                    }

                    case QScrollBar::SliderMove: {
                        rdsurface_clearselection(m_surface);
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
    this->sync_location();
}

SurfaceWidget::~SurfaceWidget() {
    rdsurface_destroy(m_surface);
    m_surface = nullptr;
}

bool SurfaceWidget::has_rdil() const { return rdsurface_hasrdil(m_surface); }

void SurfaceWidget::set_rdil(bool v) const {
    rdsurface_setrdil(m_surface, v);
    this->viewport()->update();
}

void SurfaceWidget::set_location(const RDSurfaceLocation& loc) {
    if(loc.address.valid) this->jump_to(loc.address.value);
}

bool SurfaceWidget::go_back() {
    if(rdsurface_goback(m_surface)) {
        if(!this->sync_location()) this->viewport()->update();
        Q_EMIT history_updated();
        return true;
    }

    return false;
}
bool SurfaceWidget::go_forward() {
    if(rdsurface_goforward(m_surface)) {
        if(!this->sync_location()) this->viewport()->update();
        Q_EMIT history_updated();
        return true;
    }

    return false;
}

void SurfaceWidget::clear_history() {
    rdsurface_clearhistory(m_surface);
    Q_EMIT history_updated();
}

void SurfaceWidget::jump_to(RDAddress address) {
    LIndex index;

    if(rdlisting_getindex(address, &index)) {
        if(this->is_index_visible(index)) {
            auto [start, end] = this->get_visible_range();
            rdsurface_setposition(m_surface, index - start, -1);
            this->viewport()->update();
        }
        else {
            const usize DIFF = (this->visible_rows() / 4);
            usize relidx = index;
            if(relidx > DIFF) relidx -= DIFF;

            rdsurface_setposition(m_surface, index - relidx, -1);
            this->verticalScrollBar()->setValue(relidx);
            rdsurface_seek(m_surface, relidx);
            this->viewport()->update();
        }

        Q_EMIT history_updated();
    }
}

void SurfaceWidget::jump_to_ep() {
    rdsurface_jumptoep(m_surface);
    Q_EMIT history_updated();
    this->sync_location();
}

void SurfaceWidget::invalidate() {
    this->update_scrollbars();
    this->viewport()->update();
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
        switch(e->button()) {
            case Qt::LeftButton: {
                RDSurfacePosition p = this->get_surface_coords(e->position());
                rdsurface_setposition(m_surface, p.row, p.col);
                Q_EMIT history_updated();
                this->viewport()->update();
                break;
            }

            case Qt::BackButton: this->go_back(); break;
            case Qt::ForwardButton: this->go_forward(); break;
            default: return;
        }

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
    if(!utils::handle_key_press(m_surface, e)) {
        QScrollBar* vscroll = this->verticalScrollBar();
        auto [row, col] = this->position();

        if(e->matches(QKeySequence::MoveToEndOfLine)) {
            rdsurface_setposition(m_surface, row, this->visible_columns());
        }
        else if(e->matches(QKeySequence::MoveToStartOfDocument)) {
            vscroll->setValue(0);
            return;
        }
        else if(e->matches(QKeySequence::MoveToEndOfDocument)) {
            vscroll->setValue(vscroll->value() + this->visible_rows() - 1);
            return;
        }
        else if(e->matches(QKeySequence::SelectEndOfLine)) {
            rdsurface_select(m_surface, row, this->visible_columns());
        }
        else if(e->matches(QKeySequence::SelectEndOfDocument)) {
            rdsurface_select(m_surface, this->visible_rows() - row,
                             this->visible_columns());
        }
        else if(e->matches(QKeySequence::SelectAll)) {
            rdsurface_setposition(m_surface, 0, 0);
            rdsurface_select(m_surface, this->visible_rows(),
                             this->visible_columns());
        }
        else if(e->key() == Qt::Key_Space) {
            Q_EMIT switch_view();
            return;
        }
        else {
            QAbstractScrollArea::keyPressEvent(e);
            return;
        }
    }

    this->viewport()->update();
}

void SurfaceWidget::paintEvent(QPaintEvent* e) {
    if(!m_surface) {
        QAbstractScrollArea::paintEvent(e);
        return;
    }

    // rdsurface_seek(m_surface, this->verticalScrollBar()->value());
    rdsurface_render(m_surface, this->visible_rows());

    usize nrows = rdsurface_getrowcount(m_surface);
    utils::draw_surface(m_surface, &m_document, 0, nrows);

    QPainter painter{this->viewport()};
    m_document.drawContents(&painter);

    Q_EMIT render_completed();
    statusbar::set_location(m_surface);
}

bool SurfaceWidget::event(QEvent* event) {
    if(m_surface && event->type() == QEvent::ToolTip) {
        auto* helpevent = static_cast<QHelpEvent*>(event);
        this->show_popup(helpevent->pos());
        return true;
    }

    return QAbstractScrollArea::event(event);
}

bool SurfaceWidget::can_goback() const {
    return rdsurface_cangoback(m_surface);
}

bool SurfaceWidget::can_goforward() const {
    return rdsurface_cangoforward(m_surface);
}

int SurfaceWidget::visible_columns() const {
    return qFloor(this->viewport()->width() / utils::cell_width());
}

int SurfaceWidget::visible_rows() const {
    return qFloor(this->viewport()->height() / utils::cell_height());
}

qreal SurfaceWidget::row_height() const {
    QTextBlock b = m_document.findBlockByLineNumber(0);

    if(b.isValid()) return b.layout()->boundingRect().height();

    return utils::cell_height();
}

RDSurfacePosition SurfaceWidget::get_surface_coords(QPointF pt) const {
    auto* layout = m_document.documentLayout();
    int pos = layout->hitTest(pt, Qt::FuzzyHit);

    if(pos == -1) qFatal("Invalid position");

    QTextCursor c(const_cast<QTextDocument*>(&m_document));
    c.setPosition(pos);

    return {
        c.blockNumber(),
        c.columnNumber(),
    };
}

RDSurfacePosition SurfaceWidget::position() const {
    RDSurfacePosition pos;
    rdsurface_getposition(m_surface, &pos);
    return pos;
}

RDSurfaceLocation SurfaceWidget::location() const {
    RDSurfaceLocation loc;
    rdsurface_getlocation(m_surface, &loc);
    return loc;
}

std::optional<RDAddress> SurfaceWidget::get_current_address() const {
    RDAddress address;
    if(rdsurface_getcurrentaddress(m_surface, &address)) return address;
    return std::nullopt;
}

std::optional<LIndex> SurfaceWidget::get_listing_index() const {
    LIndex lidx;
    if(rdsurface_getlistingindex(m_surface, &lidx)) return lidx;
    return std::nullopt;
}

usize SurfaceWidget::get_listing_length() const {
    return rdlisting_getlength();
}

QPair<LIndex, LIndex> SurfaceWidget::get_visible_range() const {
    LIndex vscrollstart = this->verticalScrollBar()->value();
    LIndex vscrollend = vscrollstart + this->visible_rows();
    return {vscrollstart, qMin(vscrollend, this->get_listing_length())};
}

bool SurfaceWidget::is_index_visible(LIndex index) const {
    auto [start, end] = this->get_visible_range();
    return index >= start && index < end;
}

bool SurfaceWidget::follow_under_cursor() {
    RDAddress address;

    if(rdsurface_getaddressundercursor(m_surface, &address)) {
        this->jump_to(address);
        return true;
    }

    return false;
}

void SurfaceWidget::update_scrollbars() {
    this->verticalScrollBar()->setRange(0, this->get_listing_length());
}

bool SurfaceWidget::sync_location() {
    RDSurfaceLocation loc = this->location();

    if(loc.startindex.valid) {
        auto oldvalue = static_cast<usize>(this->verticalScrollBar()->value());

        if(oldvalue != loc.startindex.value) {
            this->verticalScrollBar()->setValue(loc.startindex.value);
            return true;
        }
    }

    return false;
}

void SurfaceWidget::show_popup(const QPoint& pt) {
    if(!m_surface) return;

    RDSurfacePosition pos = this->get_surface_coords(pt);
    RDAddress address;

    if(rdsurface_getaddressunderpos(m_surface, &pos, &address) &&
       rdsurface_indexof(m_surface, address) == -1) {
        m_popup->popup(address);
        return;
    }

    m_popup->hide();
}
