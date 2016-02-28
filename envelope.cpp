#include "envelope.h"
#include <QPainter>
#include <QDebug>

Envelope::Envelope(QWidget *parent) :
    QWidget(parent),
    i(0), a(0), d(0), s(255), r(0)
{

}

/*****************************************************************************/
void Envelope::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black));

    float w = width();
    float h = height() - 2.0f;
    const float scale = 1.0f / 256.0f;

    float w4 = w * 0.25f;
    float ax = (w4 * a) * scale;
    float dx = (w4 * d) * scale;
    float rx = (w4 * r) * scale;
    float lx = w - ax - dx - w4;

    float sl = h * (1.0f - s * scale);
    float il = h * (1.0f - i * scale);
    float ml = h * (1.0f - 255.0f / 256.0f);

    QPen linePen(QColor(18, 90, 102));
    linePen.setWidth(2);

    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0f, QColor(238, 246, 246));
    gradient.setColorAt(0.6f, QColor(197, 223, 228));
    gradient.setColorAt(0.601f, QColor(128, 186, 196));
    gradient.setColorAt(1.0f, QColor(197, 223, 228));
    gradient.setCoordinateMode(QGradient::StretchToDeviceMode);

    QBrush fillBrush(gradient);
    painter.setPen(linePen);

    QTransform transform;
    transform.translate(1.0f, 1.0f);
    painter.setTransform(transform);

    QPainterPath path;
    path.moveTo(0.0f, h);
    path.lineTo(0.0f, il);
    path.lineTo(ax, ml);
    QPointF ddp(ax + dx, sl);
    QPointF cd1(ax, (ml + sl) * 0.5f);
    QPointF cd2(ax + dx * 0.5f, sl);
    path.cubicTo(cd1, cd2, ddp);
    path.lineTo(ax + dx + lx, sl);

    float rl = ax + dx + lx;
    QPointF drp(rl + rx, h);
    QPointF cr1(rl, (sl + h) * 0.5f);
    QPointF cr2(rl + rx * 0.5f, h);
    path.cubicTo(cr1, cr2, drp);
    path.lineTo(w - 1.0f, h);
    path.closeSubpath();
    painter.drawPath(path);
    painter.fillPath(path, fillBrush);



//    painter.setPen(Qt::red);
//    painter.drawPoint(cr1);
//    painter.drawPoint(cr2);
}


/*
 * QPointF p1, p2;

// Draw attack
    p1.setY(h - 1.0f);
    for (int h = 0; h < ap; h++) {
        p1.setX((float) h);
        p2.setX((float) h);
        p2.setY()
        painter.drawLine(p1, p2);
    }
    painter.dra
    */
