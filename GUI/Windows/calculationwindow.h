#ifndef CALCULATIONWINDOW_H
#define CALCULATIONWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "Core/System/system.h"
#include "Core/System/componentcomputationmap.h"

#include "GUI/Utilities/tablecombobox.h"
#include "GUI/Windows/detailswindow.h"


namespace Ui {
    class CalculationWindow;
}

class CalculationWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::CalculationWindow *ui;
    QString windowLoadStartPath;

    ComponentComputationMap *componentComputationMap;
    DetailsWindow *detailsWindow;
    System *system;

    std::vector<ComponentMeasures::Type> componentMeasuresVector;
    ComponentsVector componentsVector;

    void initComponentsLevelTableWidget();

    void initTableWidget();
    void setComponentsInTableWidget();
    void clearTableWidget();
    void updateTableWidget();

    void showDetailsWindow(Component &paComponent, ComponentMeasures::Type paComponentMeasure);
    void hideDetailsWindow();
    void deleteDetailsWindow();

    void createComponentComputationMap();
    void deleteComponentComputationMap();
    void setComponentComputationMap();

    void resetSystem();
    void initWindowTitle(const std::string *paFileName = NULL);

    void openFile(const std::string &paFileName);

private slots:
    void tableWidgetDoubleClicked(int paRow, int paColumnn);
    void resizeTableWidgetRow(int paRow, int paOldSize, int paNewSize);
    void resizeTableWidgetColumn(int paColumn, int paOldSize, int paNewSize);

    void resizeComponentsLevelTableWidgetRow(int paRow, int paOldSize, int paNewSize);
    void resizeComponentsLevelTableWidgetColumn(int paColumn, int paOldSize, int paNewSize);
    void updateComponentsLevelTableWidgetColumn(TableComboBox *paComboBox);

    void loadFile();
    void reloadFile();
    void calculate();
    void exportToCSVFile();

protected:
    virtual void closeEvent(QCloseEvent *e);

public:
    explicit CalculationWindow(bool paEqualReliability, QWidget *parent = 0);
    void showAndSetReliabilityType(bool paEqualReliability);
    virtual ~CalculationWindow();

signals:
    void closed(CalculationWindow *paCalculationWindow);
};

#endif // CALCULATIONWINDOW_H
