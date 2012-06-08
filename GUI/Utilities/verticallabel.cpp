#include "verticallabel.h"

#include <QPainter>
#include <QFontMetrics>

VerticalLabel::VerticalLabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{

}

VerticalLabel::VerticalLabel(const QString &paText, QWidget *parent, Qt::WindowFlags f):
    QLabel(paText, parent, f)
{

}

void VerticalLabel::paintEvent(QPaintEvent *e)
{
    QPainter pomPainter;
    pomPainter.begin(this);
    pomPainter.setFont(font());

    drawVerticalText(&pomPainter, text());
    pomPainter.end();
}

void VerticalLabel::drawVerticalText(QPainter *paPainter, const QString &paText)
{
    int pomWidth = (int)(fontMetrics().height());
    int pomHeight = fontMetrics().width(paText);

    setMaximumSize(pomWidth, QWIDGETSIZE_MAX);
    setMinimumSize(pomWidth, pomHeight);

    int pomX = (width() + (int)(0.75 * pomWidth)) / 2;
    int pomY = (height() + pomHeight) / 2;

    paPainter->save();
    paPainter->translate(pomX, pomY);
    paPainter->rotate(-90);
    paPainter->drawText(0, 0, paText);
    paPainter->restore();
}
