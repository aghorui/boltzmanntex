QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

LIBS += -lm

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

contains(QMAKE_HOST.os, *win32*) {
    message("Building on WIN32")
    BUILD_TIME = $$system("echo %date:~7,2%/%date:~4,2%/%date:~10,4%") # no spaces between 'system' command and args.
} else {
    BUILD_TIME = $$system("date +%d/%m/%y")
}

SHORTCOMMIT_ID = $$system("git rev-parse --short HEAD")

# Generate and write version header during build time

BUILD_INFO_HEADER = "$${LITERAL_HASH}ifndef BUILD_HPP$$escape_expand(\n)\
$${LITERAL_HASH}define BUILD_HPP$$escape_expand(\n)\
$$escape_expand(\n)\
$${LITERAL_HASH}define BOLTZMANNTEX_BUILD_NUMBER \"$$BUILD_TIME\"$$escape_expand(\n)\
$${LITERAL_HASH}define BOLTZMANNTEX_BUILD_COMMIT_DATE \"$$SHORTCOMMIT_ID\"$$escape_expand(\n)\
$$escape_expand(\n)\
$${LITERAL_HASH}endif // BUILD_HPP$$escape_expand(\n)"

write_file("build.hpp", BUILD_INFO_HEADER)

SOURCES += \
    ../dependencies/qcustomplot/qcustomplot.cpp \
    aboutdialog.cpp \
    csvimporter/columnselectentry.cpp \
    csvimporter/csvimporter.cpp \
    dataset.cpp \
    datasettablemodel.cpp \
    disclaimerdialog.cpp \
    manualdialog.cpp \
    oestemperaturecalculator/oestemperaturecalculator.cpp \
    oestemperaturecalculator/oestools.cpp \
    oestemperaturecalculator/rangetablemodel.cpp \
    main.cpp \
    mainwindow.cpp \
    reportgenerator.cpp \
    settings.cpp \
    simplegraphplot/simplegraphplot.cpp \
    subprogram.cpp \
    subprogramselector.cpp

HEADERS += \
    ../dependencies/qcustomplot/qcustomplot.h \
    aboutdialog.hpp \
    csvimporter/columnselectentry.hpp \
    csvimporter/csvimporter.hpp \
    dataset.hpp \
    datasettablemodel.hpp \
    disclaimerdialog.hpp \
    manualdialog.hpp \
    oestemperaturecalculator/oestemperaturecalculator.hpp \
    oestemperaturecalculator/oestools.hpp \
    oestemperaturecalculator/rangetablemodel.hpp \
    mainwindow.hpp \
    reportgenerator.hpp \
    settings.hpp \
    simplegraphplot/simplegraphplot.hpp \
    subprogram.hpp \
    subprogramselector.hpp \
    version.hpp \
    build.hpp

FORMS += \
    aboutdialog.ui \
    csvimporter/columnselectentry.ui \
    csvimporter/csvimporter.ui \
    disclaimerdialog.ui \
    manualdialog.ui \
    oestemperaturecalculator/oestemperaturecalculator.ui \
    mainwindow.ui \
    simplegraphplot/simplegraphplot.ui \
    subprogramselector.ui

TRANSLATIONS += \
    boltzmanntex_en_IN.ts

RELEASE_TARGET_PATH = $$OUT_PWD/../release_package

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $$RELEASE_TARGET_PATH
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += ../dependencies/qcustomplot/

RESOURCES += \
    resources/resource.qrc

DISTFILES += \
    resources/template.html

RC_ICONS = resources/icon.ico
