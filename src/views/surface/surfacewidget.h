#pragma once

#include <QAbstractScrollArea>
#include <QMenu>
#include <QTextDocument>
#include <redasm/redasm.h>

class SurfaceWidget: public QAbstractScrollArea {
    Q_OBJECT

public:
    explicit SurfaceWidget(QWidget* parent = nullptr);
    ~SurfaceWidget() override;
    [[nodiscard]] inline float cell_height() const { return m_cellheight; }
    [[nodiscard]] inline float cell_width() const { return m_cellwidth; }
    [[nodiscard]] inline RDSurface* handle() const { return m_surface; }
    [[nodiscard]] RDSurfacePosition position() const;
    [[nodiscard]] int visible_columns() const;
    [[nodiscard]] int visible_rows() const;
    [[nodiscard]] qreal row_height() const;

public Q_SLOTS:
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

private:
    [[nodiscard]] RDSurfacePosition get_surface_coords(QPointF pt) const;
    [[nodiscard]] bool get_surface_index(usize* index) const;
    [[nodiscard]] usize get_listing_length() const;
    bool follow_under_cursor();
    void create_context_menu();
    void update_scrollbars();
    void sync_location();

Q_SIGNALS:
    void render_completed();
    void switch_view();

private:
    RDSurface* m_surface{nullptr};
    QTextDocument m_document;
    float m_cellwidth, m_cellheight;
    QMenu* m_menu;
};
