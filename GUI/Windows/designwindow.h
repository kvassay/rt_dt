#ifndef DESIGNWINDOW_H
#define DESIGNWINDOW_H

#include <QMainWindow>

#include <vector>

#include "Core/SystemDesign/systemdesign.h"
#include "GUI/Utilities/tablespinbox.h"

namespace Ui {
    class DesignWindow;
}

class QTableWidgetItem;

class DesignWindow : public QMainWindow
{
    Q_OBJECT
private:
    static const int c_FIRST_STATE_COLUMN = 2;

    Ui::DesignWindow *ui;

    SystemDesign system;
    StateVector statesCountsVector;
    int tableWidgetStatesColumn;
    QString windowStartPath;

    void resetSystem();
    QString getWindowTitle(const std::string *paFileName = NULL);
    void initWindowTitle(const std::string *paFileName = NULL);
    void updateStatesCountsVector(ComponentDesign &paComponent, d_COMPONENTSCOUNT paComponentRow, d_STATESCOUNT paOldComponentStatesCount, d_STATESCOUNT paNewComponentStatesCount);
    void loadFile(const std::string &paFileName);
    bool saveFile(const std::string &paFileName);
    bool saveAsToFileWithReturn();
    bool saveToFileWithReturn();
    void addComponentsToComponentsTableWidget(d_COMPONENTSCOUNT paOldComponentsCount, d_COMPONENTSCOUNT paNewComponentsCount);
    bool tryCheckStructuralFunction();
    bool trySaveChangedSystem();

private slots:
    void changeComponentsCount(int paComponentsCount);
    void changeComponentStatesCount(TableSpinBox *paTableSpinBox);
    void changeComponentStateProbability(QTableWidgetItem *paTableWidgetItem);
    void changeSystemConfigurationState(QTableWidgetItem *paTableWidgetItem);

    void updateTableWidget();
    void checkSystem();
    void isNonDecreasing();

    void loadFromFile();
    void saveAsToFile();
    void saveToFile();

protected:
    virtual void closeEvent(QCloseEvent *e);

public:
    explicit DesignWindow(QWidget *parent = 0);
    ~DesignWindow();

signals:
    void closed(DesignWindow *paDesignWindow);
};

#endif // DESIGNWINDOW_H
