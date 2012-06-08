#include "calculationwindow.h"
#include "ui_calculationwindow.h"

#include <QFileDialog>
#include <QComboBox>
#include <QVector>
#include <QCloseEvent>
#include <QMessageBox>

#include <iostream>
#include <fstream>
#include <locale>

#include "Core/ImportanceIndices/componentmeasures.h"
#include "Core/ImportanceIndices/componentindex.h"

CalculationWindow::CalculationWindow(bool paEqualReliability, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CalculationWindow)
{
    ui->setupUi(this);
    ui->exportToCSVLevelCmb->setCurrentIndex(Third);
    windowLoadStartPath = "";

    componentComputationMap = NULL;

    ComponentMeasures::getAllTypes(componentMeasuresVector);

    system = new System(paEqualReliability);
    detailsWindow = NULL;

    QObject::connect(ui->loadBtn, SIGNAL(clicked()), this, SLOT(loadFile()));
    QObject::connect(ui->reloadBtn, SIGNAL(clicked()), this, SLOT(reloadFile()));
    QObject::connect(ui->calculateBtn, SIGNAL(clicked()), this, SLOT(calculate()));
    QObject::connect(ui->exportToCSV, SIGNAL(clicked()), this, SLOT(exportToCSVFile()));
    QObject::connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(tableWidgetDoubleClicked(int,int)));

    QObject::connect(ui->componentsLevelTableWidget->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeTableWidgetRow(int,int,int)));
    QObject::connect(ui->componentsLevelTableWidget->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeTableWidgetColumn(int,int,int)));
    QObject::connect(ui->tableWidget->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeComponentsLevelTableWidgetRow(int,int,int)));
    QObject::connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeComponentsLevelTableWidgetColumn(int,int,int)));

    initWindowTitle();
    initComponentsLevelTableWidget();
    initTableWidget();
}

CalculationWindow::~CalculationWindow()
{
    deleteDetailsWindow();
    delete system;

    delete ui;
}

void CalculationWindow::loadFile()
{
    QString pomFileNameQString = QFileDialog::getOpenFileName(this, NULL, windowLoadStartPath, tr("Text files (*.txt);;All files (*)"));
    std::string pomFileName = pomFileNameQString.toStdString();

    openFile(pomFileName);
    std::string pomDir = system->getName();
    if (pomDir != "")
        windowLoadStartPath = QString::fromStdString(pomDir);
}

void CalculationWindow::reloadFile()
{
    std::string pomFileName = system->getName();
    openFile(pomFileName);
}

void CalculationWindow::calculate()
{
    Error pomError;
    setComponentComputationMap();
    system->tryCalculate(*componentComputationMap, pomError);
    updateTableWidget();

    if (pomError)
    {
        QMessageBox pomMsgBox;
        pomMsgBox.setText(tr("Unexpected error."));
        pomMsgBox.setDetailedText(QString::fromStdString(pomError.getText()));
        pomMsgBox.setStandardButtons(QMessageBox::Close);
        pomMsgBox.setDefaultButton(QMessageBox::Close);
        pomMsgBox.exec();
    }
}

void CalculationWindow::initComponentsLevelTableWidget()
{
    std::vector<ComponentMeasures::Type> pomMeasuresVector;
    ComponentMeasures::getAllTypes(pomMeasuresVector);
    d_MEASURESCOUNT pomMeasuresVectorSize = pomMeasuresVector.size();

    QStringList pomHorizontalHeaders;
    for (d_MEASURESCOUNT i = 0; i < pomMeasuresVectorSize; i++)
    {
        pomHorizontalHeaders.append(QString::fromStdString(ComponentMeasures::toString(pomMeasuresVector[i])));
    }
    pomHorizontalHeaders.append(tr("All"));

    QTableWidget *pomTableWidget = ui->componentsLevelTableWidget;
    pomTableWidget->setColumnCount(pomMeasuresVectorSize + 1);
    pomTableWidget->setRowCount(0);
    pomTableWidget->setHorizontalHeaderLabels(pomHorizontalHeaders);
}

void CalculationWindow::initTableWidget()
{
    d_MEASURESCOUNT pomMeasuresVectorSize = componentMeasuresVector.size();

    QStringList pomHorizontalHeaders;
    for (d_MEASURESCOUNT i = 0; i < pomMeasuresVectorSize; i++)
    {
        pomHorizontalHeaders.append(QString::fromStdString(ComponentMeasures::toString(componentMeasuresVector[i])));
    }

    QTableWidget *pomTableWidget = ui->tableWidget;
    pomTableWidget->setColumnCount(pomMeasuresVectorSize);
    pomTableWidget->setRowCount(0);
    pomTableWidget->setHorizontalHeaderLabels(pomHorizontalHeaders);

    pomTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void CalculationWindow::setComponentsInTableWidget()
{
    d_COMPONENTSCOUNT pomComponentsCount = componentsVector.size();

    QStringList pomVerticalHeaders;
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        pomVerticalHeaders.append(QString::fromStdString(componentsVector[i]->getName()));
    }

    QTableWidget *pomTableWidget = ui->tableWidget;
    pomTableWidget->clearContents();
    pomTableWidget->setRowCount(pomComponentsCount);
    pomTableWidget->setVerticalHeaderLabels(pomVerticalHeaders);
}

void CalculationWindow::updateTableWidget()
{
    d_COMPONENTSCOUNT pomComponentsCount = componentsVector.size();
    d_MEASURESCOUNT pomMeasuresVectorSize = componentMeasuresVector.size();

    Error pomError;

    QTableWidget *pomTableWidget = ui->tableWidget;
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        Component *pomComponent = componentsVector[i];
        for (d_MEASURESCOUNT j = 0; j < pomMeasuresVectorSize; j++)
        {
            ComponentIndex *pomIndex = pomComponent->getComponentIndex(componentMeasuresVector[j], pomError);
            const ImportanceIndex *pomIndexValue = pomIndex->getMainIndexAvg();
            if (pomIndexValue != NULL)
            {
                pomTableWidget->setItem(i, j, new QTableWidgetItem(QString::number(pomIndexValue->getValue()), QTableWidgetItem::Type));
            }
            else
            {
                pomTableWidget->setItem(i, j, new QTableWidgetItem("", QTableWidgetItem::Type));
            }
        }
    }
}

void CalculationWindow::showDetailsWindow(Component &paComponent, ComponentMeasures::Type paComponentMeasure)
{
    if (detailsWindow == NULL)
        detailsWindow = new DetailsWindow(*system);

    detailsWindow->addComponentMeasure(paComponent, paComponentMeasure);

    if (detailsWindow->isHidden())
        detailsWindow->show();
}

void CalculationWindow::hideDetailsWindow()
{
    if (detailsWindow != NULL && detailsWindow->isVisible())
        detailsWindow->hide();
}

void CalculationWindow::deleteDetailsWindow()
{
    hideDetailsWindow();
    if (detailsWindow != NULL)
    {
        delete detailsWindow;
        detailsWindow = NULL;
    }
}

void CalculationWindow::tableWidgetDoubleClicked(int paRow, int paColumnn)
{
    showDetailsWindow(*(componentsVector[paRow]), componentMeasuresVector[paColumnn]);
}

void CalculationWindow::closeEvent(QCloseEvent *e)
{
    resetSystem();
    QMainWindow::closeEvent(e);
    emit closed(this);
}

void CalculationWindow::createComponentComputationMap()
{
    componentComputationMap = new ComponentComputationMap(*system, Third);

    std::vector<ComponentMeasures::Type> pomMeasuresVector;
    ComponentMeasures::getAllTypes(pomMeasuresVector);
    d_MEASURESCOUNT pomMeasuresVectorSize = pomMeasuresVector.size();

    ComponentsVector pomComponents;
    system->getComponents(pomComponents);
    d_COMPONENTSCOUNT pomComponentsCount = pomComponents.size();

    QStringList pomVerticalHeaders;
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        pomVerticalHeaders.append(QString::fromStdString(pomComponents[i]->getName()));
    }
    pomVerticalHeaders.append(tr("All"));

    QTableWidget *pomTableWidget = ui->componentsLevelTableWidget;
    pomTableWidget->setRowCount(pomComponentsCount + 1);
    pomTableWidget->setVerticalHeaderLabels(pomVerticalHeaders);

    Error pomError;
    for (d_COMPONENTSCOUNT i = 0; i <= pomComponentsCount; i++)
    {
        for (d_MEASURESCOUNT j = 0; j <= pomMeasuresVectorSize; j++)
        {
            TableComboBox *pomComboBox = new TableComboBox(i, j);

            pomComboBox->addItem(tr("None"));
            pomComboBox->addItem(tr("First"));
            pomComboBox->addItem(tr("Second"));
            pomComboBox->addItem(tr("Third"));
            if (i >= pomComponentsCount || j >= pomMeasuresVectorSize)
            {
                pomComboBox->addItem(tr("Custom"));
                pomComboBox->setCurrentIndex(Third + 1);
            }
            else
            {
                d_COMPONENTSCOUNT pomComponentID = pomComponents[i]->getId();
                ComponentMeasures::Type pomMeasureType = pomMeasuresVector[j];
                pomComboBox->setCurrentIndex(componentComputationMap->getLevel(pomComponentID, pomMeasureType, pomError));
            }

            pomTableWidget->setCellWidget(i, j, pomComboBox);
            QObject::connect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
        }
    }
}

void CalculationWindow::deleteComponentComputationMap()
{
    if (componentComputationMap != NULL)
    {
        delete componentComputationMap;
        componentComputationMap = NULL;
    }

    QTableWidget *pomTableWidget = ui->componentsLevelTableWidget;
    pomTableWidget->clearContents();
    pomTableWidget->setRowCount(0);
}

void CalculationWindow::setComponentComputationMap()
{
    std::vector<ComponentMeasures::Type> pomMeasuresVector;
    ComponentMeasures::getAllTypes(pomMeasuresVector);
    d_MEASURESCOUNT pomMeasuresVectorSize = pomMeasuresVector.size();

    ComponentsVector pomComponents;
    system->getComponents(pomComponents);
    d_COMPONENTSCOUNT pomComponentsCount = pomComponents.size();

    QTableWidget *pomTableWidget = ui->componentsLevelTableWidget;

    Error pomError;
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        for (d_MEASURESCOUNT j = 0; j < pomMeasuresVectorSize; j++)
        {
            d_COMPONENTSCOUNT pomComponentID = pomComponents[i]->getId();
            ComponentMeasures::Type pomMeasureType = pomMeasuresVector[j];

            TableComboBox *pomComboBox = (TableComboBox*)(pomTableWidget->cellWidget(i, j));
            componentComputationMap->setLevel(pomComponentID, pomMeasureType, (Level)(pomComboBox->currentIndex()), pomError);
        }
    }
}

void CalculationWindow::resizeTableWidgetRow(int paRow, int paOldSize, int paNewSize)
{
    QTableWidget *pomTable = ui->tableWidget;

    QObject::disconnect(pomTable->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeComponentsLevelTableWidgetRow(int,int,int)));
    pomTable->setRowHeight(paRow, paNewSize);
    QObject::connect(pomTable->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeComponentsLevelTableWidgetRow(int,int,int)));
}

void CalculationWindow::resizeTableWidgetColumn(int paColumn, int paOldSize, int paNewSize)
{
    QTableWidget *pomTable = ui->tableWidget;

    QObject::disconnect(pomTable->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeComponentsLevelTableWidgetColumn(int,int,int)));
    pomTable->setColumnWidth(paColumn, paNewSize);
    QObject::connect(pomTable->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeComponentsLevelTableWidgetColumn(int,int,int)));
}

void CalculationWindow::resizeComponentsLevelTableWidgetRow(int paRow, int paOldSize, int paNewSize)
{
    QTableWidget *pomTable = ui->componentsLevelTableWidget;

    QObject::disconnect(pomTable->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeTableWidgetRow(int,int,int)));
    pomTable->setRowHeight(paRow, paNewSize);
    QObject::connect(pomTable->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeTableWidgetRow(int,int,int)));
}

void CalculationWindow::resizeComponentsLevelTableWidgetColumn(int paColumn, int paOldSize, int paNewSize)
{
    QTableWidget *pomTable = ui->componentsLevelTableWidget;

    QObject::disconnect(pomTable->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeTableWidgetColumn(int,int,int)));
    pomTable->setColumnWidth(paColumn, paNewSize);
    QObject::connect(pomTable->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(resizeTableWidgetColumn(int,int,int)));
}

void CalculationWindow::clearTableWidget()
{
    QTableWidget *pomTable = ui->tableWidget;

    pomTable->setRowCount(0);
}

void CalculationWindow::updateComponentsLevelTableWidgetColumn(TableComboBox *paComboBox)
{
    if (paComboBox == NULL)
        return;

    QTableWidget *pomTable = ui->componentsLevelTableWidget;

    int pomLastRow = pomTable->rowCount() - 1;
    int pomLastColumn = pomTable->columnCount() - 1;

    int pomRow = paComboBox->getRow();
    int pomColumn = paComboBox->getColumn();
    int pomItemIndex = paComboBox->currentIndex();
    int pomCustomIndexValue = Third + 1;

    if (pomItemIndex > Third)
    {
        return;
    }
    else if (pomRow < pomLastRow && pomColumn < pomLastColumn)
    {
        TableComboBox *pomComboBox = (TableComboBox*)(pomTable->cellWidget(pomRow, pomLastColumn));

        QObject::disconnect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
        pomComboBox->setCurrentIndex(pomCustomIndexValue);
        QObject::connect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));

        pomComboBox = (TableComboBox*)(pomTable->cellWidget(pomLastRow, pomColumn));
        QObject::disconnect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
        pomComboBox->setCurrentIndex(pomCustomIndexValue);
        QObject::connect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));

        pomComboBox = (TableComboBox*)(pomTable->cellWidget(pomLastRow, pomLastColumn));
        QObject::disconnect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
        pomComboBox->setCurrentIndex(pomCustomIndexValue);
        QObject::connect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
    }
    else if (pomRow >= pomLastRow && pomColumn < pomLastColumn)
    {
        TableComboBox *pomComboBox;

        for (int i = 0; i < pomLastRow; i++)
        {
            pomComboBox = (TableComboBox*)(pomTable->cellWidget(i, pomColumn));
            if (pomItemIndex != pomComboBox->currentIndex())
            {
                QObject::disconnect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
                pomComboBox->setCurrentIndex(pomItemIndex);
                QObject::connect(pomComboBox, SIGNAL(itemChanged(TableComboBox*)), this, SLOT(updateComponentsLevelTableWidgetColumn(TableComboBox*)));
            }
        }

        for (int i = 0; i <= pomLastRow; i++)
        {
            pomComboBox = (TableComboBox*)(pomTable->cellWidget(i, pomLastColumn));
            if (pomItemIndex != pomComboBox->currentIndex())
            {
                pomComboBox->setCurrentIndex(pomCustomIndexValue);
            }
        }
    }
    else if (pomRow < pomLastRow && pomColumn >= pomLastColumn)
    {
        TableComboBox *pomComboBox;

        for (int j = 0; j < pomLastColumn; j++)
        {
            pomComboBox = (TableComboBox*)(pomTable->cellWidget(pomRow, j));
            if (pomItemIndex != pomComboBox->currentIndex())
            {
                pomComboBox->setCurrentIndex(pomItemIndex);
            }
        }

        for (int j = 0; j <= pomLastColumn; j++)
        {
            pomComboBox = (TableComboBox*)(pomTable->cellWidget(pomLastRow, j));
            if (pomItemIndex != pomComboBox->currentIndex())
            {
                pomComboBox->setCurrentIndex(pomCustomIndexValue);
            }
        }
    }
    else
    {
        for (int i = 0; i <= pomLastRow; i++)
        {
            for (int j = 0; j <= pomLastColumn; j++)
            {
                TableComboBox *pomComboBox = (TableComboBox*)(pomTable->cellWidget(i, j));
                if (pomItemIndex != pomComboBox->currentIndex())
                {
                    pomComboBox->setCurrentIndex(pomItemIndex);
                }
            }
        }
    }
}

void CalculationWindow::exportToCSVFile()
{
    QString pomDirName = QString::fromStdString(system->getName());
    if (pomDirName.endsWith(".txt",Qt::CaseInsensitive))
        pomDirName.remove(pomDirName.lastIndexOf('.'), 4);
    pomDirName.append(".csv");

    QString pomFileName = QFileDialog::getSaveFileName(this, NULL, pomDirName, tr("CSV files (*.csv);;Text files (*.txt);;All files (*)"));
    if (pomFileName != "")
    {
        std::vector<ComponentMeasures::Type> pomComponentMeasuresVector;
        ComponentMeasures::getAllTypes(pomComponentMeasuresVector);
        Level pomLevel = (Level)(ui->exportToCSVLevelCmb->currentIndex());

        std::ofstream pomFile;
        pomFile.open(pomFileName.toStdString().c_str());
        system->exportToCSVStream(pomLevel, pomComponentMeasuresVector,';',pomFile);
        pomFile.close();
    }
}

void CalculationWindow::resetSystem()
{
    initWindowTitle();

    deleteDetailsWindow();
    clearTableWidget();
    deleteComponentComputationMap();

    componentsVector.clear();
    system->clear();
}

void CalculationWindow::showAndSetReliabilityType(bool paEqualReliability)
{
    if (isVisible())
        return;

    if (system->hasEqualReliability() != paEqualReliability)
    {
        delete system;
        system = new System(paEqualReliability);
    }
    initWindowTitle();
    setVisible(true);
}

void CalculationWindow::initWindowTitle(const std::string *paFileName)
{
    QString pomText = tr("Calculation of importance measures");
    if (system->hasEqualReliability())
        pomText = pomText + " (A(i) = P(i))";
    else
        pomText = pomText + " (A(i) = (P(i)+P(i+1)+...+P(M-1)))";

    if (paFileName != NULL)
        pomText = pomText + ": " + " " + QString::fromStdString(*paFileName);

    setWindowTitle(pomText);
}

void CalculationWindow::openFile(const std::string &paFileName)
{
    if (paFileName != "")
    {
        resetSystem();

        Error pomError;

        std::ifstream pomFile;
        pomFile.open(paFileName.c_str());

        system->tryLoadFromTxtStream(paFileName, pomFile, true, pomError);
        pomFile.close();

        if (pomError)
        {
            initWindowTitle();

            QMessageBox pomMsgBox;
            pomMsgBox.setText(tr("The file ") + QString::fromStdString(paFileName) + tr(" has not convenient format."));
            pomMsgBox.setDetailedText(QString::fromStdString(pomError.getText()));
            pomMsgBox.setStandardButtons(QMessageBox::Close);
            pomMsgBox.setDefaultButton(QMessageBox::Close);
            pomMsgBox.exec();

            return;
        }

        ComponentsMap pomComponents;
        system->getComponents(pomComponents);
        componentsVector.resize(pomComponents.size());
        d_COMPONENTSCOUNT i = 0;
        for (ComponentsMap::iterator it = pomComponents.begin(); it != pomComponents.end(); it++)
        {
            componentsVector[i] = it->second;
            i++;
        }

        initWindowTitle(&paFileName);
        createComponentComputationMap();
        setComponentsInTableWidget();
    }
}
