#ifndef GUICONFIG_H
#define GUICONFIG_H

#include <QColor>

class GUIConfig
{
public:
    GUIConfig();

    static QColor getTableErrorCellColor() { return QColor(Qt::red); }
    static QColor getTableOKCellColor() { return QColor(Qt::white); }
    static QColor getTableEditableCellColor() { return QColor(Qt::white); }
    static QColor getTableNotEditableCellColor() { return QColor(Qt::blue); }
    static QColor getTableUsedCellColor() { return QColor(Qt::white); }
    static QColor getTableNotUsedCellColor() { return QColor(Qt::lightGray); }
};

#endif // GUICONFIG_H
