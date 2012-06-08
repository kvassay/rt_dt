#include "designwindow.h"
#include "ui_designwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

#include <limits>
#include <fstream>

#include "GUI/guiconfig.h"

const int DesignWindow::c_FIRST_STATE_COLUMN;

DesignWindow::DesignWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DesignWindow)
{
    ui->setupUi(this);

    initWindowTitle();
    windowStartPath = "";

    QTableWidget *pomTable = ui->componentsTableWidget;
    pomTable->setColumnCount(c_FIRST_STATE_COLUMN);
    pomTable->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("States count")));
    pomTable->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Probabilities sum")));

    QSpinBox *pomSpb = ui->componentsCountSpb;
    pomSpb->setMinimum(0);
    pomSpb->setMaximum(c_MAX_COMPONENTS_COUNT);
    changeComponentsCount(pomSpb->value());

    tableWidgetStatesColumn = 0;
    updateTableWidget();

    QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));
    QObject::connect(pomSpb, SIGNAL(valueChanged(int)), this, SLOT(changeComponentsCount(int)));

    QObject::connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeSystemConfigurationState(QTableWidgetItem*)));

    QObject::connect(ui->loadBtn, SIGNAL(clicked()), this, SLOT(loadFromFile()));
    QObject::connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveToFile()));
    QObject::connect(ui->saveAsBtn, SIGNAL(clicked()), this, SLOT(saveAsToFile()));
    QObject::connect(ui->updateStructuralFunctionBtn, SIGNAL(clicked()), this, SLOT(updateTableWidget()));
    QObject::connect(ui->checkBtn, SIGNAL(clicked()), this, SLOT(checkSystem()));
    QObject::connect(ui->isNonDecreasingBtn, SIGNAL(clicked()), this, SLOT(isNonDecreasing()));
}

DesignWindow::~DesignWindow()
{
    delete ui;
}

void DesignWindow::closeEvent(QCloseEvent *e)
{
    if (trySaveChangedSystem())
    {
        resetSystem();

        QMainWindow::closeEvent(e);
        emit closed(this);
    }
    else
    {
        e->ignore();
    }
}

void DesignWindow::changeComponentsCount(int paComponentsCount)
{
    d_COMPONENTSCOUNT pomNewComponentsCount;
    if (paComponentsCount < 0)
        pomNewComponentsCount = 0;
    else
        pomNewComponentsCount = paComponentsCount;

    d_COMPONENTSCOUNT pomOldComponentsCount = system.getComponentsCount();

    QTableWidget *pomTable = ui->componentsTableWidget;
    for (d_COMPONENTSCOUNT i = pomNewComponentsCount; i < pomOldComponentsCount; i++)
    {
        TableSpinBox *pomSpb = (TableSpinBox*)(pomTable->cellWidget(i, 0));
        QObject::disconnect(pomSpb, SIGNAL(valueChanged(TableSpinBox*)), this, SLOT(changeComponentStatesCount(TableSpinBox*)));

        ComponentDesign *pomComponent = system.getComponentByNumber(i + 1);
        updateStatesCountsVector(*pomComponent, i, pomComponent->getStatesCount(), 0);
    }

    Error pomError;
    system.setComponentsCount(pomNewComponentsCount, pomError);
    pomNewComponentsCount = system.getComponentsCount();

    addComponentsToComponentsTableWidget(pomOldComponentsCount, pomNewComponentsCount);

    if (pomError)
    {
        QMessageBox pomMsgBox;
        pomMsgBox.setText("Too much components.");
        pomMsgBox.setDetailedText(QString::fromStdString(pomError.getText()));
        pomMsgBox.setStandardButtons(QMessageBox::Close);
        pomMsgBox.setDefaultButton(QMessageBox::Close);
        pomMsgBox.exec();

        QTableWidget *pomTable = ui->componentsTableWidget;
        QSpinBox *pomSpb = ui->componentsCountSpb;

        QObject::disconnect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));
        pomSpb->setValue(pomNewComponentsCount);
        QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));
    }
}

void DesignWindow::changeComponentStatesCount(TableSpinBox *paTableSpinBox)
{
    if (paTableSpinBox != NULL)
    {
        d_COMPONENTSCOUNT pomComponentRow = paTableSpinBox->getRow();
        ComponentDesign *pomComponent = system.getComponentByNumber(pomComponentRow + 1);
        d_STATESCOUNT pomOldCount = pomComponent->getStatesCount();
        pomComponent->setStatesCount(paTableSpinBox->value());
        d_STATESCOUNT pomNewCount = pomComponent->getStatesCount();

        updateStatesCountsVector(*pomComponent, pomComponentRow, pomOldCount, pomNewCount);
    }
}

void DesignWindow::updateStatesCountsVector(ComponentDesign &paComponent, d_COMPONENTSCOUNT paComponentRow,
                                            d_STATESCOUNT paOldComponentStatesCount, d_STATESCOUNT paNewComponentStatesCount)
{
    QTableWidget *pomTable = ui->componentsTableWidget;

    QObject::disconnect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));

    QTableWidgetItem *pomSumItem = pomTable->item(paComponentRow, 1);
    pomSumItem->setText(QString::number(paComponent.getProbabilitiesSum()));
    if (CHECK_PROBABILITIES_SUM(paComponent.getProbabilitiesSum()))
    {
        pomSumItem->setBackground(QBrush(GUIConfig::getTableOKCellColor()));
    }
    else
    {
        pomSumItem->setBackground(QBrush(GUIConfig::getTableErrorCellColor()));
    }

    QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));


    d_STATESCOUNT pomOldVectorSize = statesCountsVector.size();
    if (statesCountsVector.size() < paNewComponentStatesCount)
        statesCountsVector.resize(paNewComponentStatesCount, 0);
    if (statesCountsVector.size() < 1)
        return;

    for (d_STATESCOUNT j = paOldComponentStatesCount; j < paNewComponentStatesCount; j++)
        statesCountsVector[j]++;
    for (d_STATESCOUNT j = paNewComponentStatesCount; j < paOldComponentStatesCount; j++)
        statesCountsVector[j]--;

    d_STATESCOUNT pomJ = statesCountsVector.size() - 1;
    while (pomJ > 0 && statesCountsVector[pomJ] <= 0)
        pomJ--;
    if (statesCountsVector[pomJ] == 0)
        statesCountsVector.resize(pomJ);
    else
        statesCountsVector.resize(pomJ + 1);
    d_STATESCOUNT pomNewVectorSize = statesCountsVector.size();

    pomTable->setColumnCount(c_FIRST_STATE_COLUMN + pomNewVectorSize);
    for (d_STATESCOUNT j = pomOldVectorSize; j < pomNewVectorSize; j++)
        pomTable->setHorizontalHeaderItem(c_FIRST_STATE_COLUMN + j, new QTableWidgetItem("P(" + QString::number(j) + ")"));


    QObject::disconnect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));

    d_COMPONENTSCOUNT pomComponentsCount = system.getComponentsCount();
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        for (d_STATESCOUNT j = pomOldVectorSize; j < pomNewVectorSize; j++)
        {
            QTableWidgetItem *pomItem = new QTableWidgetItem();
            pomTable->setItem(i, c_FIRST_STATE_COLUMN + j, pomItem);
            pomItem->setBackground(QBrush(GUIConfig::getTableNotEditableCellColor()));
            pomItem->setFlags(Qt::NoItemFlags);
            pomItem->setText("");
        }
    }
    for (d_STATESCOUNT j = paOldComponentStatesCount; j < paNewComponentStatesCount; j++)
    {
        QTableWidgetItem *pomItem = pomTable->item(paComponentRow, c_FIRST_STATE_COLUMN + j);
        if (pomItem == NULL)
        {
            pomItem = new QTableWidgetItem();
            pomTable->setItem(paComponentRow, c_FIRST_STATE_COLUMN + j, pomItem);
        }
        pomItem->setBackground(QBrush(GUIConfig::getTableEditableCellColor()));
        pomItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
        pomItem->setText(QString::number(paComponent.getProbability(j)));
    }
    for (d_STATESCOUNT j = paNewComponentStatesCount; j < pomNewVectorSize; j++)
    {
        QTableWidgetItem *pomItem = pomTable->item(paComponentRow, c_FIRST_STATE_COLUMN + j);
        if (pomItem == NULL)
        {
            pomItem = new QTableWidgetItem();
            pomTable->setItem(paComponentRow, c_FIRST_STATE_COLUMN + j, pomItem);
        }
        pomItem->setBackground(QBrush(GUIConfig::getTableNotEditableCellColor()));
        pomItem->setFlags(Qt::NoItemFlags);
        pomItem->setText("");
    }

    QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));
}

void DesignWindow::changeComponentStateProbability(QTableWidgetItem *paTableWidgetItem)
{
    d_COMPONENTSCOUNT pomRow = paTableWidgetItem->row();
    d_STATESCOUNT pomColumn = paTableWidgetItem->column();

    if (pomColumn < c_FIRST_STATE_COLUMN)
    {
        return;
    }

    QString pomText = paTableWidgetItem->text().trimmed();
    if (pomText == "")
        pomText = "0";

    bool pomOK;
    d_PROBABILITY pomProbability = pomText.toDouble(&pomOK);
    ComponentDesign *pomComponent = system.getComponentByNumber(pomRow + 1);
    QTableWidget *pomTable = ui->componentsTableWidget;

    if (!pomOK || pomProbability < 0 || pomProbability > 1)
    {
        QMessageBox pomMsg;
        pomMsg.setText(tr("The probability is a number from 0 to 1."));
        pomMsg.exec();
    }
    else
    {
        pomComponent->trySetProbability(pomColumn - c_FIRST_STATE_COLUMN, pomProbability);
    }
    QObject::disconnect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));
    paTableWidgetItem->setText(QString::number(pomComponent->getProbability(pomColumn - c_FIRST_STATE_COLUMN)));
    QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeComponentStateProbability(QTableWidgetItem*)));

    QTableWidgetItem *pomSumItem = pomTable->item(pomRow, 1);
    pomSumItem->setText(QString::number(pomComponent->getProbabilitiesSum()));
    if (CHECK_PROBABILITIES_SUM(pomComponent->getProbabilitiesSum()))
    {
        pomSumItem->setBackground(QBrush(GUIConfig::getTableOKCellColor()));
    }
    else
    {
        pomSumItem->setBackground(QBrush(GUIConfig::getTableErrorCellColor()));
    }
}

void DesignWindow::loadFromFile()
{
    if (trySaveChangedSystem())
    {
        QString pomFileNameQString = QFileDialog::getOpenFileName(this, NULL, windowStartPath, tr("Text files (*.txt);;All files (*)"));
        std::string pomFileName = pomFileNameQString.toStdString();

        loadFile(pomFileName);
        std::string pomDir = system.getName();
        if (pomDir != "")
            windowStartPath = QString::fromStdString(pomDir);
    }
}

void DesignWindow::saveAsToFile()
{
    saveAsToFileWithReturn();
}

void DesignWindow::saveToFile()
{
    saveToFileWithReturn();
}

bool DesignWindow::saveAsToFileWithReturn()
{
    if (!tryCheckStructuralFunction())
        return false;

    //QString windowStartPath = QString::fromStdString(system.getName());
    QString pomFileNameQString = QFileDialog::getSaveFileName(this, NULL, windowStartPath, tr("Text files (*.txt);;All files (*)"));
    std::string pomFileName = pomFileNameQString.toStdString();

    if (saveFile(pomFileName))
    {
        windowStartPath = QString::fromStdString(system.getName());
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignWindow::saveToFileWithReturn()
{
    if (system.getName() != "")
    {
        if (tryCheckStructuralFunction())
            return saveFile(system.getName());
        else
            return false;
    }
    else
    {
        return saveAsToFileWithReturn();
    }
}

bool DesignWindow::tryCheckStructuralFunction()
{
    if (!system.isStructuralFunctionUpdated())
    {
        QMessageBox pomMsgBox;
        pomMsgBox.setText(tr("The structural function is not updated."));
        pomMsgBox.setStandardButtons(QMessageBox::Close);
        pomMsgBox.setDefaultButton(QMessageBox::Close);
        pomMsgBox.exec();

        return false;
    }
    return true;
}

bool DesignWindow::trySaveChangedSystem()
{
    if (system.isChanged())
    {
        QMessageBox pomMsgBox;
        std::string pomSystemName = system.getName();
        pomMsgBox.setWindowTitle(getWindowTitle(&pomSystemName));
        pomMsgBox.setText(tr("Do you want to save changes?"));
        pomMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        pomMsgBox.setDefaultButton(QMessageBox::Yes);
        int pomResult = pomMsgBox.exec();

        if (pomResult == QMessageBox::Cancel)
            return false;
        else if (pomResult == QMessageBox::Yes)
        {
            if (!saveToFileWithReturn())
                return false;
        }
    }
    return true;
}

void DesignWindow::resetSystem()
{
    initWindowTitle();
    ui->componentsCountSpb->setValue(0);
    system.clear();
    updateTableWidget();
}

void DesignWindow::updateTableWidget()
{
    Error pomError;
    if (!system.tryUpdateStructuralFunction(pomError))
    {
        QMessageBox pomMsgBox;
        pomMsgBox.setText(tr("Too much system configuration states."));
        pomMsgBox.setDetailedText(QString::fromStdString(pomError.getText()));
        pomMsgBox.setStandardButtons(QMessageBox::Close);
        pomMsgBox.setDefaultButton(QMessageBox::Close);
        pomMsgBox.exec();

        return;
    }

    d_COMPONENTSCOUNT pomComponentsCount = system.getComponentsCount();
    d_SYSTEMCONFIGURATIONSCOUNT pomConfigCounts = system.getConfigurationsCount();

    QTableWidget *pomTable = ui->tableWidget;
    QObject::disconnect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeSystemConfigurationState(QTableWidgetItem*)));

    tableWidgetStatesColumn = pomComponentsCount + 1;
    pomTable->setColumnCount(pomComponentsCount + 2);
    pomTable->setHorizontalHeaderItem(pomComponentsCount, new QTableWidgetItem(tr("Probability")));
    pomTable->setHorizontalHeaderItem(pomComponentsCount + 1, new QTableWidgetItem(tr("State")));

    for (d_COMPONENTSCOUNT j = 0; j < pomComponentsCount; j++)
    {
        ComponentDesign *pomComponent = system.getComponentByNumber(j + 1);

        QTableWidgetItem *pomHeaderItem = pomTable->horizontalHeaderItem(j);
        if (pomHeaderItem == NULL)
            pomTable->setHorizontalHeaderItem(j, new QTableWidgetItem(QString::fromStdString(pomComponent->getName())));
        else
            pomHeaderItem->setText(QString::fromStdString(pomComponent->getName()));
    }

    pomTable->setRowCount(pomConfigCounts);
    StateVector pomStatesVector;
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomConfigCounts; i++)
    {
        system.configurationNumberToStatesVector(i, pomStatesVector);
        for (d_COMPONENTSCOUNT j = 0; j < pomComponentsCount; j++)
        {
            QTableWidgetItem *pomItem = pomTable->item(i, j);
            if (pomItem == NULL)
            {
                pomItem = new QTableWidgetItem(QString::number(pomStatesVector[j]));
                pomTable->setItem(i, j, pomItem);
            }
            else
            {
                pomItem->setText(QString::number(pomStatesVector[j]));
            }
            pomItem->setBackground(QBrush(GUIConfig::getTableOKCellColor()));
            pomItem->setFlags(Qt::NoItemFlags);
        }
        {
            QTableWidgetItem *pomItem;
            QString pomStr;

            pomItem = pomTable->item(i, pomComponentsCount);
            pomStr = QString::number(system.getConfigurationProbability(i));
            if (pomItem == NULL)
            {
                pomItem = new QTableWidgetItem(pomStr);
                pomTable->setItem(i, pomComponentsCount, pomItem);
            }
            else
            {
                pomItem->setText(pomStr);
            }
            pomItem->setBackground(QBrush(GUIConfig::getTableOKCellColor()));
            pomItem->setFlags(Qt::NoItemFlags);

            pomItem = pomTable->item(i, tableWidgetStatesColumn);
            pomStr = QString::number(system.getConfigurationState(i));
            if (pomItem == NULL)
            {
                pomItem = new QTableWidgetItem(pomStr);
                pomTable->setItem(i, tableWidgetStatesColumn, pomItem);
            }
            else
            {
                pomItem->setText(pomStr);
            }
            pomItem->setBackground(QBrush(GUIConfig::getTableEditableCellColor()));
            pomItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
        }
    }

    QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeSystemConfigurationState(QTableWidgetItem*)));
}

void DesignWindow::loadFile(const std::string &paFileName)
{
    if (paFileName != "")
    {
        resetSystem();

        Error pomError;

        std::ifstream pomFile;
        pomFile.open(paFileName.c_str());

        system.tryLoadFromTxtStream(paFileName, pomFile, pomError);
        pomFile.close();

        if (pomError)
        {
            initWindowTitle();

            QMessageBox pomMsgBox;
            pomMsgBox.setText(tr("The file ") + QString::fromStdString(paFileName) + tr(" has not a convenient format."));
            pomMsgBox.setDetailedText(QString::fromStdString(pomError.getText()));
            pomMsgBox.setStandardButtons(QMessageBox::Close);
            pomMsgBox.setDefaultButton(QMessageBox::Close);
            pomMsgBox.exec();

            return;
        }

        initWindowTitle(&paFileName);
        addComponentsToComponentsTableWidget(0, system.getComponentsCount());
        updateTableWidget();
        ui->componentsCountSpb->setValue(system.getComponentsCount());
    }
}

bool DesignWindow::saveFile(const std::string &paFileName)
{
    if (paFileName != "")
    {
        Error pomError;

        std::ofstream pomFile;
        pomFile.open(paFileName.c_str());

        system.trySaveToTxtStream(paFileName, pomFile, pomError);
        pomFile.close();

        std::string pomName = system.getName();
        initWindowTitle(&pomName);

        if (pomError)
        {
            QMessageBox pomMsgBox;
            pomMsgBox.setText(tr("Unexpected error."));
            pomMsgBox.setDetailedText(QString::fromStdString(pomError.getText()));
            pomMsgBox.setStandardButtons(QMessageBox::Close);
            pomMsgBox.setDefaultButton(QMessageBox::Close);
            pomMsgBox.exec();

            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

QString DesignWindow::getWindowTitle(const std::string *paFileName)
{
    QString pomText = tr("System design");
    if (paFileName != NULL && *paFileName != "")
        pomText = pomText + ": " + QString::fromStdString(*paFileName);
    return pomText;
}

void DesignWindow::initWindowTitle(const std::string *paFileName)
{
    setWindowTitle(getWindowTitle(paFileName));
}

void DesignWindow::addComponentsToComponentsTableWidget(d_COMPONENTSCOUNT paOldComponentsCount, d_COMPONENTSCOUNT paNewComponentsCount)
{
    QTableWidget *pomTable = ui->componentsTableWidget;
    pomTable->setRowCount(paNewComponentsCount);
    for (d_COMPONENTSCOUNT i = paOldComponentsCount; i < paNewComponentsCount; i++)
    {
        ComponentDesign *pomComponent = system.getComponentByNumber(i + 1);
        TableSpinBox *pomSpb = new TableSpinBox(i, 0);
        pomSpb->setMaximum(c_MAX_COMPONENT_STATES_COUNT);
        pomSpb->setMinimum(c_MIN_COMPONENT_STATES_COUNT);
        pomSpb->setValue(pomComponent->getStatesCount());
        pomTable->setCellWidget(i, 0, pomSpb);

        QTableWidgetItem *pomItem = new QTableWidgetItem();
        pomItem->setFlags(Qt::NoItemFlags);
        pomTable->setItem(i, 1, pomItem);

        pomTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString::fromStdString(pomComponent->getName())));

        updateStatesCountsVector(*pomComponent, i, 0, pomComponent->getStatesCount());

        QObject::connect(pomSpb, SIGNAL(valueChanged(TableSpinBox*)), this, SLOT(changeComponentStatesCount(TableSpinBox*)));
    }
}

void DesignWindow::changeSystemConfigurationState(QTableWidgetItem *paTableWidgetItem)
{
    if (paTableWidgetItem == NULL)
        return;

    d_SYSTEMCONFIGURATIONSCOUNT pomRow = paTableWidgetItem->row();
    d_SYSTEMCONFIGURATIONSCOUNT pomColumn = paTableWidgetItem->column();

    if (pomColumn != tableWidgetStatesColumn)
        return;

    QTableWidget *pomTable = ui->tableWidget;
    bool pomOK;
    int pomStateInt = paTableWidgetItem->text().toInt(&pomOK);
    if (!pomOK || pomStateInt < 0 || (d_STATESCOUNT)pomStateInt >= c_MAX_SYSTEM_STATES_COUNT)
    {
        QMessageBox pomMsg;
        pomMsg.setText(tr("The state is an integer from 0 to ") + QString::number(c_MAX_SYSTEM_STATES_COUNT - 1) + ".");
        pomMsg.exec();

        QObject::disconnect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeSystemConfigurationState(QTableWidgetItem*)));
        paTableWidgetItem->setText(QString::number(system.getConfigurationState(pomRow)));
        QObject::connect(pomTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeSystemConfigurationState(QTableWidgetItem*)));

        return;
    }

    d_STATESCOUNT pomState = (d_STATESCOUNT)pomStateInt;
    system.setConfigurationState(pomRow, pomState);
    paTableWidgetItem->setText(QString::number(system.getConfigurationState(pomRow)));
}

void DesignWindow::checkSystem()
{
    QMessageBox pomMsg;
    d_COMPONENTSCOUNT pomComponentsCount = system.getComponentsCount();

    if (pomComponentsCount <= 0)
    {
        pomMsg.setText(tr("No component set."));
        pomMsg.exec();
        return;
    }
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        ComponentDesign *pomComponent = system.getComponentByNumber(i+1);
        if (!CHECK_PROBABILITIES_SUM(pomComponent->getProbabilitiesSum()))
        {
            pomMsg.setText(tr("The sum of probabilities of states of component with id = ") + QString::number(pomComponent->getId()) + tr(" is different from 1."));
            pomMsg.exec();
            return;
        }
    }
    if (!system.isStructuralFunctionUpdated())
    {
        pomMsg.setText(tr("The structural function is not updated."));
        pomMsg.exec();
        return;
    }

    pomMsg.setText(tr("The system is OK."));
    pomMsg.exec();
}

void DesignWindow::isNonDecreasing()
{
    QMessageBox pomMsg;

    if (!system.isStructuralFunctionUpdated())
    {
        pomMsg.setText(tr("The structural function is not updated."));
        pomMsg.exec();
        return;
    }

    if (system.getComponentsCount() <= 0)
    {
        return;
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomDown;
    d_SYSTEMCONFIGURATIONSCOUNT pomUp;
    Error pomError;
    if (!system.isStructuralFunctionNonDecreasing(pomDown, pomUp, pomError))
    {
        if (pomError)
        {
            pomMsg.setText(tr("Unexpected error."));
            pomMsg.setDetailedText(QString::fromStdString(pomError.getText()));
            pomMsg.setStandardButtons(QMessageBox::Close);
            pomMsg.setDefaultButton(QMessageBox::Close);
            pomMsg.exec();

            return;
        }
        else
        {
            pomMsg.setText(tr("The structural function is decreasing.") + " " +
                           tr("Check state vectors with numbers") + " " +
                           QString::number(pomDown + 1) + " " + tr("and") + " " +
                           QString::number(pomUp + 1) + ".");
            pomMsg.setDetailedText(QString::fromStdString(pomError.getText()));
            pomMsg.setStandardButtons(QMessageBox::Close);
            pomMsg.setDefaultButton(QMessageBox::Close);
            pomMsg.exec();
            return;
        }
    }

    pomMsg.setText(tr("The structural function system is non-decreasing."));
    pomMsg.exec();
}

