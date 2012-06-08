#include "detailswindow.h"

#include <QTableWidget>
#include <QPushButton>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QCloseEvent>

#include "Core/System/system.h"
#include "Core/ImportanceIndices/componentmeasures.h"

#include "GUI/Utilities/verticallabel.h"

DetailsWindow::DetailsWindow(System &paSystem, QWidget *parent) :
    QWidget(parent)
{
    ComponentsMap pomComponentsMap;
    paSystem.getComponents(pomComponentsMap);
    d_COMPONENTSCOUNT pomComponentsCount = pomComponentsMap.size();
    d_COMPONENTSCOUNT pomRow = 0;
    for (ComponentsMap::iterator it = pomComponentsMap.begin(); it != pomComponentsMap.end(); it++)
    {
        componentsIDMap.insert(ComponentsIDMap::value_type(it->second->getId(), pomRow));
        pomRow++;
    }

    std::vector<ComponentMeasures::Type> pomMeasuresVector;
    ComponentMeasures::getAllTypes(pomMeasuresVector);
    d_MEASURESCOUNT pomMeasuresVectorSize = pomMeasuresVector.size();
    d_MEASURESCOUNT pomColumn = 0;
    for (d_MEASURESCOUNT i = 0; i < pomMeasuresVectorSize; i++)
    {
        measuresTypeMap.insert(MeasuresTypeMap::value_type(pomMeasuresVector[i], pomColumn));
        pomColumn++;
    }

    detailsMatrix.resize(pomComponentsCount, pomMeasuresVectorSize);

    gridColumnVector.resize(pomComponentsCount, 0);
    dataLayout = new QGridLayout();

    std::string pomName = paSystem.getName();
    initWindowTitle(&pomName, paSystem.hasEqualReliability());

    QLabel *pomHorizontalLabel = new QLabel(tr("States of the system"));
    QFont pomHorizontalFont;
    pomHorizontalFont.setBold(true);
    pomHorizontalLabel->setAlignment(Qt::AlignCenter);
    pomHorizontalLabel->setFont(pomHorizontalFont);
    //pomHorizontalLabel->setStyleSheet("QLabel { background-color : red; color : blue; }");

    VerticalLabel *pomVerticalLabel = new VerticalLabel(tr("States of the components"));
    QFont pomVerticalFont;
    pomVerticalFont.setBold(true);
    pomVerticalLabel->setFont(pomVerticalFont);

    QWidget *pomViewport = new QWidget();
    pomViewport->setLayout(dataLayout);

    QScrollArea *pomScrollArea = new QScrollArea;
    pomScrollArea->setWidget(pomViewport);
    pomScrollArea->setWidgetResizable(true);

    QGridLayout *pomMainLayout = new QGridLayout();
    pomMainLayout->addWidget(pomHorizontalLabel, 0, 1);
    pomMainLayout->addWidget(pomVerticalLabel, 1, 0);
    pomMainLayout->addWidget(pomScrollArea,1,1);


    setLayout(pomMainLayout);
}

void DetailsWindow::initWindowTitle(const std::string *paFileName, bool paEqualReliability)
{
    QString pomText = tr("Details");
    if (paEqualReliability)
        pomText = pomText + " (A(i) = P(i))";
    else
        pomText = pomText + " (A(i) = (P(i)+P(i+1)+...+P(M-1)))";

    if (paFileName != NULL)
        pomText = pomText + ": " + " " + QString::fromStdString(*paFileName);

    setWindowTitle(pomText);
}

void DetailsWindow::addComponentMeasure(Component &paComponent, ComponentMeasures::Type paMeasureType)
{
    d_COMPONENTSCOUNT pomRow;
    if (!tryGetComponentRow(paComponent, pomRow))
        return;

    d_MEASURESCOUNT pomColumn;
    if (!tryGetMeasureTypeColumn(paMeasureType, pomColumn))
        return;

    bool pomOK;
    DetailsWidget *pomDetailsWidget = detailsMatrix.get(pomRow, pomColumn, pomOK);
    if (!pomOK)
    {
        return;
    }

    if (pomDetailsWidget == NULL)
    {
        pomDetailsWidget = new DetailsWidget(paComponent, paMeasureType);
        detailsWidgetManager.registerDetailsWidget(pomDetailsWidget);
        detailsMatrix.trySet(pomRow, pomColumn, pomDetailsWidget);
        dataLayout->addWidget(pomDetailsWidget, pomRow, pomColumn);
        if (gridColumnVector[pomRow] == 0)
        {
            dataLayout->setRowStretch(pomRow, pomDetailsWidget->getVerticalStretch());
        }
        gridColumnVector[pomRow]++;

        QObject::connect(pomDetailsWidget, SIGNAL(closed(DetailsWidget*)), this, SLOT(deleteDetailsWidget(DetailsWidget*)));
    }

}

void DetailsWindow::deleteDetailsWidget(DetailsWidget * paDetailsWidget)
{
    if (paDetailsWidget != NULL)
    {
        d_COMPONENTSCOUNT pomRow;
        Component *pomComponent = paDetailsWidget->getComponent();
        if (tryGetComponentRow(*pomComponent, pomRow))
        {
            d_MEASURESCOUNT pomColumn;
            if (tryGetMeasureTypeColumn(paDetailsWidget->getComponentMeasureType(), pomColumn))
            {
                detailsMatrix.trySet(pomRow, pomColumn, NULL);
            }

            gridColumnVector[pomRow]--;
            if (gridColumnVector[pomRow] == 0)
            {
                dataLayout->setRowStretch(pomRow, 0);
            }
        }
        detailsWidgetManager.unregisterDetailsWidget(paDetailsWidget);

        dataLayout->removeWidget(paDetailsWidget);
        delete paDetailsWidget;
    }
}

bool DetailsWindow::tryGetComponentRow(Component &paComponent, d_COMPONENTSCOUNT &paRow)
{
    d_COMPONENTSCOUNT pomComponentID = paComponent.getId();
    ComponentsIDMap::iterator pomComponentIt = componentsIDMap.find(pomComponentID);
    if (pomComponentIt == componentsIDMap.end())
    {
        return false;
    }
    paRow = pomComponentIt->second;
    return true;
}

bool DetailsWindow::tryGetMeasureTypeColumn(ComponentMeasures::Type paMeasureType, d_MEASURESCOUNT &paColumn)
{
    MeasuresTypeMap::iterator pomMeasureIt = measuresTypeMap.find(paMeasureType);
    if (pomMeasureIt == measuresTypeMap.end())
    {
        return false;
    }
    paColumn = pomMeasureIt->second;
    return true;
}

void DetailsWindow::closeEvent(QCloseEvent *e)
{
    e->setAccepted(false);
    hide();
}

void DetailsWindow::showEvent(QShowEvent *e)
{
    resize(width(),height());
}
