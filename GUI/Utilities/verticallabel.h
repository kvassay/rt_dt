#ifndef VERTICALLABEL_H
#define VERTICALLABEL_H

#include <QLabel>

class VerticalLabel : public QLabel
{
    Q_OBJECT

protected:
    virtual void paintEvent(QPaintEvent *e);
    void drawVerticalText(QPainter *paPainter, const QString &paText);

public:
    explicit VerticalLabel(QWidget *parent = 0, Qt::WindowFlags f = 0);
    explicit VerticalLabel(const QString &paText, QWidget * parent = 0, Qt::WindowFlags f = 0);
};

#endif // VERTICALLABEL_H
