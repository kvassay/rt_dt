#-------------------------------------------------
#
# Project created by QtCreator 2011-10-19T21:59:54
#
#-------------------------------------------------

QT       += core gui

TARGET = rt_dt
TEMPLATE = app


SOURCES += main.cpp\
    Core/System/structuralfunction.cpp \
    Core/System/component.cpp \
    Core/System/system.cpp \
    Core/error.cpp \
    Core/System/dplr.cpp \
    Core/ImportanceIndices/componentindex.cpp \
    Core/ImportanceIndices/componentmeasures.cpp \
    Core/System/componentcomputationmap.cpp \
    Core/ImportanceIndices/importanceindex.cpp \
    Core/ImportanceIndices/rtstructuralindex.cpp \
    Core/ImportanceIndices/rtstructuralmodifiedindex.cpp \
    Core/ImportanceIndices/dplrstructuralmodifiedindex.cpp \
    Core/ImportanceIndices/dplrstructuralindex.cpp \
    Core/ImportanceIndices/rtbirnbaumindex.cpp \
    Core/ImportanceIndices/dplrbirnbaumindex.cpp \
    Core/ImportanceIndices/rtcriticalityindex.cpp \
    Core/ImportanceIndices/dplrcriticalityindex.cpp \
    Core/ImportanceIndices/rcbirnbaumindex.cpp \
    GUI/Utilities/flatpushbutton.cpp \
    GUI/Utilities/verticallabel.cpp \
    GUI/Utilities/tablecombobox.cpp \
    GUI/Widgets/detailswidget.cpp \
    GUI/Windows/mainwindow.cpp \
    GUI/Windows/calculationwindow.cpp \
    GUI/Windows/detailswindow.cpp \
    GUI/Other/detailswidgetmanager.cpp \
    GUI/Windows/designwindow.cpp \
    Core/SystemDesign/componentdesign.cpp \
    Core/SystemDesign/systemdesign.cpp \
    GUI/Utilities/tablespinbox.cpp \
    Core/SystemBase/componentbase.cpp \
    Core/SystemDesign/structuralfunctiondesign.cpp

HEADERS  += Core/System/structuralfunction.h \
    Core/System/component.h \
    Core/System/system.h \
    Core/definitions.h \
    Core/error.h \
    Core/errorcodes.h \
    Core/System/dplr.h \
    Core/ImportanceIndices/componentindex.h \
    Core/ImportanceIndices/componentmeasures.h \
    Core/System/componentcomputationmap.h \
    Core/ImportanceIndices/importanceindex.h \
    Core/Utilities/matrix.h \
    Core/ImportanceIndices/rtstructuralindex.h \
    Core/ImportanceIndices/enums.h \
    Core/ImportanceIndices/rtstructuralmodifiedindex.h \
    Core/Utilities/matrixwithbitmap.h \
    Core/ImportanceIndices/dplrstructuralmodifiedindex.h \
    Core/ImportanceIndices/dplrstructuralindex.h \
    Core/ImportanceIndices/rtbirnbaumindex.h \
    Core/ImportanceIndices/dplrbirnbaumindex.h \
    Core/ImportanceIndices/rtcriticalityindex.h \
    Core/ImportanceIndices/dplrcriticalityindex.h \
    Core/ImportanceIndices/rcbirnbaumindex.h \
    Core/Utilities/vectorwithbitmap.h \
    GUI/Utilities/flatpushbutton.h \
    GUI/Utilities/verticallabel.h \
    GUI/Utilities/tablecombobox.h \
    GUI/Widgets/detailswidget.h \
    GUI/Windows/mainwindow.h \
    GUI/Windows/calculationwindow.h \
    GUI/Windows/detailswindow.h \
    GUI/Other/detailswidgetmanager.h \
    GUI/Windows/designwindow.h \
    Core/SystemDesign/componentdesign.h \
    Core/SystemDesign/systemdesign.h \
    GUI/Utilities/tablespinbox.h \
    GUI/guiconfig.h \
    Core/SystemBase/componentbase.h \
    Core/SystemDesign/structuralfunctiondesign.h

FORMS    += GUI/Windows/calculationwindow.ui \
    GUI/Windows/mainwindow.ui \
    GUI/Windows/designwindow.ui

RESOURCES += \
    resources.qrc
