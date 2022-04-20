/*
 * Program Main Window
 * Copyright (C) 2022 Anamitra Ghorui
 *
 * This file is part of BoltzmannTex.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QLayout>
#include <QCloseEvent>
#include <QSettings>

#include "subprogram.hpp"
#include "manualdialog.hpp"
#include "aboutdialog.hpp"
#include "disclaimerdialog.hpp"
#include "oestemperaturecalculator/oestemperaturecalculator.hpp"
#include "simplegraphplot/simplegraphplot.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int currentSubProgramId = -1;
    Subprogram *currentSubProgram = nullptr;
    QWidget *subprogramSelector = nullptr;

public slots:
    void show();
    void close();
    void setSaveStatus(bool status);
    void setSubProgram(int id);
    void closeEvent(QCloseEvent *event);
    void openManual(QString anchor);

private slots:
    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

    void on_actionTutorial_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_Project_triggered();

    void on_actionNew_Project_triggered();

    void on_actionSave_As_triggered();

private:
    Ui::MainWindow *ui;
    bool saveStatus = true;
    QString savePath = "";
    QString projectTitle = "BoltzmannTex";

    // TODO allocate dynamically
    DisclaimerDialog disclaimer;
    ManualDialog manual;
};

#endif // MAINWINDOW_HPP
