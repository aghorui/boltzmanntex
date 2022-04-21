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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , disclaimer(this)
    , manual(this)
{
    ui->setupUi(this);
    connect(ui->subprogramSelector, SIGNAL(done(int)), this, SLOT(setSubProgram(int)));
}

void MainWindow::setSaveStatus(bool status)
{
    if (status == saveStatus)
        return;

    if (!status) {
        setWindowTitle(projectTitle + " (Modified)");
    } else {
        setWindowTitle(projectTitle);
    }

    saveStatus = status;
}

void MainWindow::openManual(QString anchor)
{
    manual.openAtAnchor(anchor);
}

void MainWindow::setSubProgram(int id)
{
    switch (id) {
    case OESTemperatureCalculator::subprogramId:
        currentSubProgram = new OESTemperatureCalculator(this);
        setCentralWidget(currentSubProgram);
        break;

    case SimpleGraphPlot::subprogramId:
        currentSubProgram = new SimpleGraphPlot(this);
        setCentralWidget(currentSubProgram);
        break;

    default:
        qDebug() << "Invalid subprogram?";
        break;
    }

    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);

    connect(currentSubProgram, SIGNAL(saveStatusUpdated(bool)),
            this, SLOT(setSaveStatus(bool)));
    connect(currentSubProgram, SIGNAL(manualRequest(QString)),
            this, SLOT(openManual(QString)));
}

void MainWindow::show()
{
    QMainWindow::show();
    disclaimer.show();
}

void MainWindow::close()
{
    disclaimer.close();
    QMainWindow::close();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionQuit_triggered()
{
    emit close();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog about(this);
    about.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ret =
            QMessageBox::question(this, tr("Please Confirm..."),
                tr("Are you sure you want to quit?\n"),
                QMessageBox::No | QMessageBox::Yes,
                QMessageBox::Yes);
    if (ret != QMessageBox::Yes) {
        event->ignore();
    } else {
        disclaimer.close();
        QMainWindow::close();
        event->accept();
    }
}

void MainWindow::on_actionTutorial_triggered()
{
    manual.show();
}

void MainWindow::on_actionSave_triggered()
{
    if (savePath.isEmpty()) {
        savePath = QFileDialog::getSaveFileName(this, "Save Project", "",
                                                "BoltzmannTex Project (*.boltzmanntex *.csv *.data *.raw);;"
                                                "JSON Files (*json);;All Files (*)");
        if (savePath.isEmpty()) {
            return;
        }
        if (QFileInfo(savePath).suffix().isEmpty()) {
            savePath.append(".boltzmanntex");
        }
    }
    currentSubProgram->save(savePath);
}

void MainWindow::on_actionOpen_Project_triggered()
{
    if (!saveStatus) {
        QMessageBox::StandardButton ret =
                QMessageBox::question(this, tr("Confirmation"),
                    tr("Are you sure you want to start a new project? You have not saved the current project.\n"),
                    QMessageBox::No | QMessageBox::Yes,
                    QMessageBox::Yes);
        if (ret != QMessageBox::Yes) {
            return;
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "*.boltzmanntex");

    if (fileName.isEmpty()) {
        return;
    }

    if (currentSubProgram == nullptr || currentSubProgramId != currentSubProgram->subprogramId) {
        QFile loadFile(fileName);

        if (!loadFile.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "Error", "Failed to open " + fileName);
            return;
        }

        int id = (QJsonDocument::fromJson(loadFile.readAll())).object()["header"].toObject()["subprogram"].toInt();
        loadFile.close();

        if (id == 0) {
            currentSubProgram = new OESTemperatureCalculator(this);

            try {
                currentSubProgram->load(fileName);
            } catch (std::invalid_argument &e) {
                QMessageBox::critical(this, "Error", "Invalid or Corrupted Project File: " + fileName);
                delete currentSubProgram;
                return;
            }

            setCentralWidget(currentSubProgram);
            ui->actionSave->setEnabled(true);
            ui->actionSave_As->setEnabled(true);
            connect(currentSubProgram, SIGNAL(saveStatusUpdated(bool)),
                    this, SLOT(setSaveStatus(bool)));
            connect(currentSubProgram, SIGNAL(manualRequest(QString)),
                    this, SLOT(openManual(QString)));
            return;
        } else {
           QMessageBox::critical(this, "Error", "Invalid or Corrupted Project File: " + fileName);
           qDebug() << "Subprogram not found: " << id;
        }
    } else {
        currentSubProgram->load(fileName);
    }
}

void MainWindow::on_actionNew_Project_triggered()
{
    if (currentSubProgram == nullptr) {
        return;
    }

    if (!saveStatus) {
        QMessageBox::StandardButton ret =
                QMessageBox::question(this, tr("Confirmation"),
                    tr("Are you sure you want to start a new project? You have not saved the current project.\n"),
                    QMessageBox::No | QMessageBox::Yes,
                    QMessageBox::Yes);
        if (ret != QMessageBox::Yes) {
            return;
        }
    }

    ui->actionSave->setEnabled(false);
    ui->actionSave_As->setEnabled(false);
    subprogramSelector = new SubprogramSelector(this);
    saveStatus = true;
    delete currentSubProgram;
    currentSubProgram = nullptr;
    setCentralWidget(subprogramSelector);
    connect(subprogramSelector, SIGNAL(done(int)), this, SLOT(setSubProgram(int)));
    setWindowTitle("BoltzmannTex");
}

void MainWindow::on_actionSave_As_triggered()
{
    savePath = QFileDialog::getSaveFileName(this, "Save Project", "",
                                            "BoltzmannTex Project (*.boltzmanntex *.csv *.data *.raw);;"
                                            "JSON Files (*.json);;All Files (*)");
    if (savePath.isEmpty()) {
        return;
    }

    if (QFileInfo(savePath).suffix().isEmpty()) {
        savePath.append(".boltzmanntex");
    }

    currentSubProgram->save(savePath);
}
