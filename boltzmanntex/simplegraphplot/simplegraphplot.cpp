/*
 * Simple Graph Plotter Subprogram
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

#include "simplegraphplot.hpp"
#include "ui_simplegraphplot.h"

SimpleGraphPlot::SimpleGraphPlot(QWidget *parent) :
    Subprogram(parent),
    ui(new Ui::SimpleGraphPlot)
{
    ui->setupUi(this);
    ui->plotter->addGraph();
    ui->plotter->xAxis->setLabel("X");
    ui->plotter->yAxis->setLabel("Y");
    ui->plotter->setInteraction(QCP::iRangeDrag, true);
    ui->plotter->setInteraction(QCP::iRangeZoom, true);
}

SimpleGraphPlot::~SimpleGraphPlot()
{
    delete ui;
}

void SimpleGraphPlot::on_applyButton_clicked()
{
    ui->plotter->xAxis->setLabel(ui->xRangeLabel->text());
    ui->plotter->yAxis->setLabel(ui->yRangeLabel->text());
    ui->plotter->replot();
}

void SimpleGraphPlot::on_loadGraphButton_clicked()
{
    CSVImporter csv;

    csv.setDatasetInputFields({"X Range", "Y Range"});

    int ret = csv.exec();
    if (ret == QDialog::Rejected) {
        return;
    }

    currentDataset = csv.getDataset();
    ui->filePath->setText(csv.getCurrentPath());
    ui->plotter->graph(0)->setData(currentDataset[0].data, currentDataset[1].data);
    ui->plotter->rescaleAxes();
    ui->plotter->replot();
}

void SimpleGraphPlot::on_saveGraphButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", "", "*.png");

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".png");
    }

    ui->plotter->savePng(fileName, 500, 500);
}
