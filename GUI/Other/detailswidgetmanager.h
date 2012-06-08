#ifndef DETAILSWIDGETMANAGER_H
#define DETAILSWIDGETMANAGER_H

#include <map>

#include "GUI/Widgets/detailswidget.h"

class DetailsWidgetManager : public QObject
{
    Q_OBJECT
private:
    typedef std::map< DetailsWidget*, DetailsWidget* > DetailsWidgetMap;
    typedef std::map< ComponentMeasures::Type, DetailsWidgetMap* > MeasureDetailsWidgetMap;
    typedef std::map< Component*, MeasureDetailsWidgetMap* > ComponentMeasureDetailsWidgetMap;
    typedef std::map< Component*, DetailsWidgetMap* > ComponentDetailsWidgetMap;
    typedef std::map< ComponentMeasures::Type, ComponentDetailsWidgetMap* > MeasureComponentDetailsWidgetMap;

    ComponentMeasureDetailsWidgetMap componentMeasureDetailsWidgetMap;
    MeasureComponentDetailsWidgetMap measureComponentDetailsWidgetMap;

private slots:
    void resizeTablesColumn(DetailsWidget* paDetailsWidget, int paColumn, int paNewSize);
    void resizeTablesRow(DetailsWidget* paDetailsWidget, int paRow, int paNewSize);

public:
    DetailsWidgetManager();

    void registerDetailsWidget(DetailsWidget* paDetailsWidget);
    void unregisterDetailsWidget(DetailsWidget* paDetailsWidget);
};

#endif // DETAILSWIDGETMANAGER_H
