#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>

const size_t MainWindow::c_MAX_WINDOWS_STACK_SIZE;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint);
    QObject::connect(ui->createSystemBtn, SIGNAL(clicked()), this, SLOT(openDesignWindow()));
    QObject::connect(ui->openCalculationEqualReliabilityBtn, SIGNAL(clicked()), this, SLOT(openCalculationEqualReliabilityWindow()));
    QObject::connect(ui->openCalculationGreaterReliabilityBtn, SIGNAL(clicked()), this, SLOT(openCalculationGreaterReliabilityWindow()));
    QObject::connect(ui->closeBtn, SIGNAL(clicked()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    tryDeleteAllWindows();
    delete ui;
}

void MainWindow::openDesignWindow()
{
    DesignWindow* pomNewWindow;
    if (designWindowsStack.size() > 0)
    {
        pomNewWindow = (DesignWindow*)(designWindowsStack.top());
        designWindowsStack.pop();
    }
    else
    {
        pomNewWindow = new DesignWindow();
    }
    windowsMap.insert(WindowsMap::value_type(pomNewWindow, pomNewWindow));
    pomNewWindow->show();

    QObject::connect(pomNewWindow, SIGNAL(closed(DesignWindow*)), this, SLOT(removeDesignWindow(DesignWindow*)));
}

void MainWindow::openCalculationEqualReliabilityWindow()
{
    CalculationWindow* pomNewWindow;
    if (calculationWindowsStack.size() > 0)
    {
        pomNewWindow = (CalculationWindow*)(calculationWindowsStack.top());
        calculationWindowsStack.pop();
        pomNewWindow->showAndSetReliabilityType(true);
    }
    else
    {
        pomNewWindow = new CalculationWindow(true);
        pomNewWindow->show();
    }
    windowsMap.insert(WindowsMap::value_type(pomNewWindow, pomNewWindow));

    QObject::connect(pomNewWindow, SIGNAL(closed(CalculationWindow*)), this, SLOT(removeCalculationWindow(CalculationWindow*)));
}

void MainWindow::openCalculationGreaterReliabilityWindow()
{
    CalculationWindow* pomNewWindow;
    if (calculationWindowsStack.size() > 0)
    {
        pomNewWindow = (CalculationWindow*)(calculationWindowsStack.top());
        calculationWindowsStack.pop();
        pomNewWindow->showAndSetReliabilityType(false);
    }
    else
    {
        pomNewWindow = new CalculationWindow(false);
        pomNewWindow->show();
    }
    windowsMap.insert(WindowsMap::value_type(pomNewWindow, pomNewWindow));

    QObject::connect(pomNewWindow, SIGNAL(closed(CalculationWindow*)), this, SLOT(removeCalculationWindow(CalculationWindow*)));
}

bool MainWindow::tryDeleteAllWindows()
{
    bool pomResult = true;
    WindowsStack closingWindows;
    for (WindowsMap::iterator it = windowsMap.begin(); it != windowsMap.end(); ++it)
    {
        QMainWindow* pomWindow = it->second;
        closingWindows.push(pomWindow);
    }
    while (closingWindows.size() > 0)
    {
        QMainWindow *pomWindow = closingWindows.top();
        closingWindows.pop();
        if (!pomWindow->close())
            pomResult = false;
    }

    while (designWindowsStack.size() > 0)
    {
        deleteTopFromDesignWindowsStack();
    }
    while (calculationWindowsStack.size() > 0)
    {
        deleteTopFromCalculationWindowsStack();
    }

    return pomResult;
}

void MainWindow::deleteTopFromDesignWindowsStack()
{
    QMainWindow *pomWindow = designWindowsStack.top();
    designWindowsStack.pop();
    delete pomWindow;
}

void MainWindow::deleteTopFromCalculationWindowsStack()
{
    QMainWindow *pomWindow = calculationWindowsStack.top();
    calculationWindowsStack.pop();
    delete pomWindow;
}

void MainWindow::removeDesignWindow(DesignWindow *paWindow)
{
    if (paWindow == NULL)
        return;

    windowsMap.erase(paWindow);
    QObject::disconnect(paWindow, SIGNAL(closed(DesignWindow*)), this, SLOT(removeDesignWindow(DesignWindow*)));

    if (designWindowsStack.size() >= c_MAX_WINDOWS_STACK_SIZE)
    {
        deleteTopFromDesignWindowsStack();
    }
    designWindowsStack.push(paWindow);
}

void MainWindow::removeCalculationWindow(CalculationWindow *paWindow)
{
    if (paWindow == NULL)
        return;

    windowsMap.erase(paWindow);
    QObject::disconnect(paWindow, SIGNAL(closed(CalculationWindow*)), this, SLOT(removeCalculationWindow(CalculationWindow*)));

    if (calculationWindowsStack.size() >= c_MAX_WINDOWS_STACK_SIZE)
    {
        deleteTopFromCalculationWindowsStack();
    }
    calculationWindowsStack.push(paWindow);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (!tryDeleteAllWindows())
        e->ignore();
    else
        QMainWindow::closeEvent(e);
}

