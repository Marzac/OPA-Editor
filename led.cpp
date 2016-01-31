/**
    OPA Editor: LED windget
    A basic LED GUI widget

    The MIT License (MIT)

    Source code copyright (c) 2013-2016 Frédéric Meslin
    Email: fredericmeslin@hotmail.com
    Website: www.fredslab.net
    Twitter: @marzacdev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#include "led.h"
#include <QPainter>

Led::Led(QWidget *parent) :
    QWidget(parent),
    luminosity(0)
{
}

/*****************************************************************************/
void Led::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black));

    int r = 128 + (((255 - 128) * luminosity) >> 16);
    QColor light = QColor(r, 0, 0);
    painter.setBrush(QBrush(light));

    float cw = width() * 0.5f;
    float ch = height() * 0.5f;
    QPointF center = QPointF(cw, ch);
    painter.drawEllipse(center, cw - 2.0f, ch - 2.0f);

    QColor dot = QColor(255, 200, 200);
    painter.setPen(QPen(dot));
    QPointF dot1 = QPointF(cw - 3.0f, ch - 2.0f);
    painter.drawPoint(dot1);
    QPointF dot2 = QPointF(cw - 2.0f, ch - 3.0f);
    painter.drawPoint(dot2);
    QPointF dot3 = QPointF(cw - 3.0f, ch - 1.0f);
    painter.drawPoint(dot3);
}
