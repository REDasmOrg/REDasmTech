#pragma once

#include <QTextDocument>
#include <QWidget>
#include <redasm/redasm.h>

class SurfacePopup: public QWidget {
    Q_OBJECT

private:
    static constexpr int N_ROWS = 10;
    static constexpr int N_COLS = 100;

public:
    explicit SurfacePopup(QWidget* parent = nullptr);
    ~SurfacePopup() override;
    bool popup(RDAddress address);

private:
    void more_rows();
    void less_rows();
    void render();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void paintEvent(QPaintEvent* e) override;

private:
    RDSurface* m_surface;
    int m_nrows{N_ROWS}, m_ncols{N_COLS};
    QTextDocument m_document;
    QPointF m_lastpos;
};
