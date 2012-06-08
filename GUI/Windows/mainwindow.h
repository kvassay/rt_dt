#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <map>
#include <stack>

#include "GUI/Windows/calculationwindow.h"
#include "GUI/Windows/designwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    typedef std::map<QMainWindow*, QMainWindow*> WindowsMap;
    typedef std::stack<QMainWindow*> WindowsStack;
    static const size_t c_MAX_WINDOWS_STACK_SIZE = 1;

    Ui::MainWindow *ui;

    WindowsMap windowsMap;
    WindowsStack designWindowsStack;
    WindowsStack calculationWindowsStack;

    bool tryDeleteAllWindows();
    void deleteTopFromDesignWindowsStack();
    void deleteTopFromCalculationWindowsStack();

private slots:
    void openDesignWindow();
    void openCalculationEqualReliabilityWindow();
    void openCalculationGreaterReliabilityWindow();
    void removeDesignWindow(DesignWindow *paWindow);
    void removeCalculationWindow(CalculationWindow *paWindow);

protected:
    virtual void closeEvent(QCloseEvent *e);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
