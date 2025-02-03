#pragma once

#include "surfacepopup.h"
#include <QAbstractScrollArea>
#include <QMenu>
#include <QTextDocument>
#include <redasm/redasm.h>

class SurfaceWidget: public QAbstractScrollArea {
    Q_OBJECT

public:
    explicit SurfaceWidget(QWidget* parent = nullptr);
    ~SurfaceWidget() override;
    [[nodiscard]] RDSurface* handle() const { return m_surface; }
    [[nodiscard]] RDSurfacePosition position() const;
    [[nodiscard]] RDSurfaceLocation location() const;
    [[nodiscard]] bool can_goback() const;
    [[nodiscard]] bool can_goforward() const;
    [[nodiscard]] int visible_columns() const;
    [[nodiscard]] int visible_rows() const;
    [[nodiscard]] qreal row_height() const;
    [[nodiscard]] bool has_rdil() const;
    void set_rdil(bool v) const;
    void set_location(const RDSurfaceLocation& loc);

public Q_SLOTS:
    bool go_back();
    bool go_forward();
    void clear_history();
    void jump_to(RDAddress address);
    void jump_to_ep();
    void invalidate();

protected:
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    bool event(QEvent* event) override;

private:
    [[nodiscard]] RDSurfacePosition get_surface_coords(QPointF pt) const;
    [[nodiscard]] bool get_surface_address(RDAddress* address) const;
    [[nodiscard]] usize get_listing_length() const;
    [[nodiscard]] QPair<LIndex, LIndex> get_visible_range() const;
    [[nodiscard]] bool is_index_visible(LIndex index) const;
    bool follow_under_cursor();
    void update_scrollbars();
    bool sync_location();
    void show_popup(const QPoint& pt);

Q_SIGNALS:
    void history_updated();
    void render_completed();
    void switch_view();

private:
    SurfacePopup* m_popup;
    RDSurface* m_surface{nullptr};
    QTextDocument m_document;
    QMenu* m_menu;
};
