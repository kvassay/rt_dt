#include "detailswidgetmanager.h"

DetailsWidgetManager::DetailsWidgetManager()
{
}

void DetailsWidgetManager::registerDetailsWidget(DetailsWidget *paDetailsWidget)
{
    if (paDetailsWidget == NULL)
        return;

    Component *pomComponent = paDetailsWidget->getComponent();
    ComponentMeasures::Type pomMeasureType = paDetailsWidget->getComponentMeasureType();

    DetailsWidgetMap *pomDetailsWidgetMap;
    MeasureComponentDetailsWidgetMap::iterator pomMeasureComponentIt = measureComponentDetailsWidgetMap.find(pomMeasureType);
    if (pomMeasureComponentIt != measureComponentDetailsWidgetMap.end())
    {
        ComponentDetailsWidgetMap *pomComponentDetailsWidgetMap = pomMeasureComponentIt->second;
        ComponentDetailsWidgetMap::iterator pomComponentFirstIt = pomComponentDetailsWidgetMap->begin();
        ComponentDetailsWidgetMap::iterator pomComponentIt = pomComponentDetailsWidgetMap->find(pomComponent);
        if (pomComponentIt != pomComponentDetailsWidgetMap->end())
        {
            pomDetailsWidgetMap = pomComponentIt->second;
        }
        else
        {
            pomDetailsWidgetMap = new DetailsWidgetMap();
            pomComponentDetailsWidgetMap->insert(ComponentDetailsWidgetMap::value_type(pomComponent, pomDetailsWidgetMap));
        }

        if (pomComponentFirstIt != pomComponentDetailsWidgetMap->end())
        {
            DetailsWidgetMap::iterator pomIt2 = pomComponentFirstIt->second->begin();
            if (pomIt2 != pomDetailsWidgetMap->end())
            {
                DetailsWidget *pomFirstDetailsWidget = pomIt2->second;
                paDetailsWidget->setColumnsBy(pomFirstDetailsWidget);
            }
        }
    }
    else
    {
        pomDetailsWidgetMap = new DetailsWidgetMap();
        ComponentDetailsWidgetMap *pomComponentDetailsWidgetMap = new ComponentDetailsWidgetMap();
        pomComponentDetailsWidgetMap->insert(ComponentDetailsWidgetMap::value_type(pomComponent, pomDetailsWidgetMap));
        measureComponentDetailsWidgetMap.insert(MeasureComponentDetailsWidgetMap::value_type(pomMeasureType, pomComponentDetailsWidgetMap));
    }
    DetailsWidgetMap::iterator pomDetailsIt = pomDetailsWidgetMap->find(paDetailsWidget);
    if (pomDetailsIt == pomDetailsWidgetMap->end())
    {
        pomDetailsWidgetMap->insert(DetailsWidgetMap::value_type(paDetailsWidget, paDetailsWidget));
    }


    ComponentMeasureDetailsWidgetMap::iterator pomComponentMeasureIt = componentMeasureDetailsWidgetMap.find(pomComponent);
    if (pomComponentMeasureIt != componentMeasureDetailsWidgetMap.end())
    {
        MeasureDetailsWidgetMap *pomMeasureDetailsWidgetMap = pomComponentMeasureIt->second;
        MeasureDetailsWidgetMap::iterator pomMeasureFirstIt = pomMeasureDetailsWidgetMap->begin();
        MeasureDetailsWidgetMap::iterator pomMeasureIt = pomMeasureDetailsWidgetMap->find(pomMeasureType);
        if (pomMeasureIt != pomMeasureDetailsWidgetMap->end())
        {
            pomDetailsWidgetMap = pomMeasureIt->second;
        }
        else
        {
            pomDetailsWidgetMap = new DetailsWidgetMap();
            pomMeasureDetailsWidgetMap->insert(MeasureDetailsWidgetMap::value_type(pomMeasureType, pomDetailsWidgetMap));
        }

        if (pomMeasureFirstIt != pomMeasureDetailsWidgetMap->end())
        {
            DetailsWidgetMap::iterator pomIt2 = pomMeasureFirstIt->second->begin();
            if (pomIt2 != pomDetailsWidgetMap->end())
            {
                DetailsWidget *pomFirstDetailsWidget = pomIt2->second;
                paDetailsWidget->setRowsBy(pomFirstDetailsWidget);
            }
        }
    }
    else
    {
        pomDetailsWidgetMap = new DetailsWidgetMap();
        MeasureDetailsWidgetMap *pomMeasureDetailsWidgetMap = new MeasureDetailsWidgetMap();
        pomMeasureDetailsWidgetMap->insert(MeasureDetailsWidgetMap::value_type(pomMeasureType, pomDetailsWidgetMap));
        componentMeasureDetailsWidgetMap.insert(ComponentMeasureDetailsWidgetMap::value_type(pomComponent, pomMeasureDetailsWidgetMap));
    }
    DetailsWidgetMap::iterator pomDetailsIt2 = pomDetailsWidgetMap->find(paDetailsWidget);
    if (pomDetailsIt2 == pomDetailsWidgetMap->end())
    {
        pomDetailsWidgetMap->insert(DetailsWidgetMap::value_type(paDetailsWidget, paDetailsWidget));
    }

    QObject::connect(paDetailsWidget, SIGNAL(tableColumnResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesColumn(DetailsWidget*,int,int)));
    QObject::connect(paDetailsWidget, SIGNAL(tableRowResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesRow(DetailsWidget*,int,int)));
}

void DetailsWidgetManager::unregisterDetailsWidget(DetailsWidget *paDetailsWidget)
{
    if (paDetailsWidget == NULL)
        return;

    Component *pomComponent = paDetailsWidget->getComponent();
    ComponentMeasures::Type pomMeasureType = paDetailsWidget->getComponentMeasureType();

    MeasureComponentDetailsWidgetMap::iterator pomMeasureComponentIt = measureComponentDetailsWidgetMap.find(pomMeasureType);
    if (pomMeasureComponentIt != measureComponentDetailsWidgetMap.end())
    {
        ComponentDetailsWidgetMap *pomComponentDetailsWidgetMap = pomMeasureComponentIt->second;
        ComponentDetailsWidgetMap::iterator pomComponentIt = pomComponentDetailsWidgetMap->find(pomComponent);
        if (pomComponentIt != pomComponentDetailsWidgetMap->end())
        {
            DetailsWidgetMap *pomDetailsWidgetMap = pomComponentIt->second;
            pomDetailsWidgetMap->erase(paDetailsWidget);
            if (pomDetailsWidgetMap->size() <= 0)
            {
                pomComponentDetailsWidgetMap->erase(pomComponentIt);
                delete pomDetailsWidgetMap;
            }

            if (pomComponentDetailsWidgetMap->size() <= 0)
            {
                measureComponentDetailsWidgetMap.erase(pomMeasureComponentIt);
                delete pomComponentDetailsWidgetMap;
            }
        }
    }

    ComponentMeasureDetailsWidgetMap::iterator pomComponentMeasureIt = componentMeasureDetailsWidgetMap.find(pomComponent);
    if (pomComponentMeasureIt != componentMeasureDetailsWidgetMap.end())
    {
        MeasureDetailsWidgetMap *pomMeasureDetailsWidgetMap = pomComponentMeasureIt->second;
        MeasureDetailsWidgetMap::iterator pomMeasureIt = pomMeasureDetailsWidgetMap->find(pomMeasureType);
        if (pomMeasureIt != pomMeasureDetailsWidgetMap->end())
        {
            DetailsWidgetMap *pomDetailsWidgetMap = pomMeasureIt->second;
            pomDetailsWidgetMap->erase(paDetailsWidget);
            if (pomDetailsWidgetMap->size() <= 0)
            {
                pomMeasureDetailsWidgetMap->erase(pomMeasureIt);
                delete pomDetailsWidgetMap;
            }

            if (pomMeasureDetailsWidgetMap->size() <= 0)
            {
                componentMeasureDetailsWidgetMap.erase(pomComponentMeasureIt);
                delete pomMeasureDetailsWidgetMap;
            }
        }
    }

    QObject::disconnect(paDetailsWidget, SIGNAL(tableColumnResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesColumn(DetailsWidget*,int,int)));
    QObject::disconnect(paDetailsWidget, SIGNAL(tableRowResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesRow(DetailsWidget*,int,int)));
}

void DetailsWidgetManager::resizeTablesColumn(DetailsWidget* paDetailsWidget, int paColumn, int paNewSize)
{
    if (paDetailsWidget == NULL)
        return;

    MeasureComponentDetailsWidgetMap::iterator pomIt = measureComponentDetailsWidgetMap.find(paDetailsWidget->getComponentMeasureType());
    ComponentDetailsWidgetMap *pomComponentDetailsWidgetMap = pomIt->second;
    for (ComponentDetailsWidgetMap::iterator it = pomComponentDetailsWidgetMap->begin(); it != pomComponentDetailsWidgetMap->end(); it++)
    {
        DetailsWidgetMap *pomDetailsWidgetMap = it->second;

        for (DetailsWidgetMap::iterator it2 = pomDetailsWidgetMap->begin(); it2 != pomDetailsWidgetMap->end(); it2++)
        {
            DetailsWidget *pomDetailsWidget = it2->second;
            if (pomDetailsWidget != paDetailsWidget)
            {
                QObject::disconnect(paDetailsWidget, SIGNAL(tableColumnResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesColumn(DetailsWidget*,int,int)));
                pomDetailsWidget->resizeTableColumn(paColumn, paNewSize);
                QObject::connect(paDetailsWidget, SIGNAL(tableColumnResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesColumn(DetailsWidget*,int,int)));
            }
        }
    }
}

void DetailsWidgetManager::resizeTablesRow(DetailsWidget* paDetailsWidget, int paRow, int paNewSize)
{
    if (paDetailsWidget == NULL)
        return;

    ComponentMeasureDetailsWidgetMap::iterator pomIt = componentMeasureDetailsWidgetMap.find(paDetailsWidget->getComponent());
    MeasureDetailsWidgetMap *pomMeasureDetailsWidgetMap = pomIt->second;
    for (MeasureDetailsWidgetMap::iterator it = pomMeasureDetailsWidgetMap->begin(); it != pomMeasureDetailsWidgetMap->end(); it++)
    {
        DetailsWidgetMap *pomDetailsWidgetMap = it->second;

        for (DetailsWidgetMap::iterator it2 = pomDetailsWidgetMap->begin(); it2 != pomDetailsWidgetMap->end(); it2++)
        {
            DetailsWidget *pomDetailsWidget = it2->second;
            if (pomDetailsWidget != paDetailsWidget)
            {
                QObject::disconnect(paDetailsWidget, SIGNAL(tableRowResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesRow(DetailsWidget*,int,int)));
                pomDetailsWidget->resizeTableRow(paRow, paNewSize);
                QObject::connect(paDetailsWidget, SIGNAL(tableRowResized(DetailsWidget*,int,int)), this, SLOT(resizeTablesRow(DetailsWidget*,int,int)));
            }
        }
    }
}
