#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>

#include <map>
#include <vector>

#include "Core/System/component.h"
#include "Core/ImportanceIndices/componentmeasures.h"
#include "Core/Utilities/matrix.h"
#include "GUI/Widgets/detailswidget.h"
#include "GUI/Other/detailswidgetmanager.h"

class DetailsWindow : public QWidget
{
    Q_OBJECT
private:
    typedef std::map<d_COMPONENTSCOUNT, d_COMPONENTSCOUNT> ComponentsIDMap;
    typedef std::map<d_COMPONENTSCOUNT, d_COMPONENTSCOUNT> MeasuresTypeMap;
    typedef std::vector<d_MEASURESCOUNT> GridColumnVector;

    ComponentsIDMap componentsIDMap;
    MeasuresTypeMap measuresTypeMap;
    Matrix<DetailsWidget*> detailsMatrix;
    DetailsWidgetManager detailsWidgetManager;

    GridColumnVector gridColumnVector;
    QGridLayout *dataLayout;

    void initWindowTitle(const std::string *paFileName, bool paEqualReliability);

    bool tryGetComponentRow(Component &paComponent, d_COMPONENTSCOUNT &paRow);
    bool tryGetMeasureTypeColumn(ComponentMeasures::Type paMeasureType, d_MEASURESCOUNT &paColumn);

private slots:
    void deleteDetailsWidget(DetailsWidget *paDetailsWidget);

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void showEvent(QShowEvent *e);

public:
    DetailsWindow(System &paSystem, QWidget *parent = 0);
    virtual ~DetailsWindow() { }

    void addComponentMeasure(Component &paComponent, ComponentMeasures::Type paMeasureType);
};

#endif // DETAILSWINDOW_H
