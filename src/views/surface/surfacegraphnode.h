#pragma once

#include "../graphview/graphviewnode.h"
#include <QTextDocument>
#include <redasm/redasm.h>

class SurfaceGraphNode: public GraphViewNode {
    Q_OBJECT

public:
    explicit SurfaceGraphNode(RDSurface* surface,
                              const RDFunctionBasicBlock& fbb, RDGraphNode n,
                              RDFunction* f, QWidget* parent = nullptr);
    [[nodiscard]] bool contains_address(RDAddress address) const;
    [[nodiscard]] int current_row() const override;
    [[nodiscard]] QSize size() const override;
    void get_surface_pos(const QPointF& pt, RDSurfacePosition* pos) const;
    void render(QPainter* painter, usize state) override;
    void update_document();

protected:
    void mousedoubleclick_event(QMouseEvent*) override;
    void mousepress_event(QMouseEvent* e) override;
    void mousemove_event(QMouseEvent* e) override;

private:
    [[nodiscard]] int start_row() const;

Q_SIGNALS:
    void follow_requested();

private:
    RDFunctionBasicBlock m_basicblock;
    QTextDocument m_document;
    RDSurface* m_surface;
};
