/*
 * OES Temperature Calculator Subprogram
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


#ifndef OESTEMPERATURECALCULATOR_HPP
#define OESTEMPERATURECALCULATOR_HPP

#include <exception>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include <QFrame>
#include <QPainter>
#include <QTextStream>
#include <QString>
#include <QVector>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonObject>

#include "subprogram.hpp"
#include "dataset.hpp"
#include "oestools.hpp"
#include "rangetablemodel.hpp"
#include "qcustomplot.h"
#include "reportgenerator.hpp"
#include "csvimporter/csvimporter.hpp"

namespace Ui {
class OESTemperatureCalculator;
}

class OESTemperatureCalculator : public Subprogram
{
    Q_OBJECT

public:
    static int const subprogramId = 0;
    static constexpr const char * const subprogramVersion = "0.5";

    explicit OESTemperatureCalculator(QWidget *parent = nullptr);
    ~OESTemperatureCalculator();

    QCPGraph *spectralPlot;
    QCPGraph *peakLinePlot;
    QCPGraph *regressionPlot;
    QCPGraph *fitLinePlot;

    QVector<QCPItemText *> regressionLineLabels;
    bool editRange = false;
    bool rangeStarted = false;
    bool spectrometerDataLoaded = false;
    bool regressionLock = false; // TODO remove later

    bool peakLinesVisible = true;

    double rangeStart;
    QCPItemStraightLine *cursorLine;
    QCPItemStraightLine *rangeStartLine;

    Dataset spectrometerData;
    Dataset peakList;
    Dataset materialData;
    OESTools::IntegrationResult integrationResult;
    OESTools::RegressionResult regressionResult;

    QVector<bool> discardedPeaks;
    DatasetTableModel peakListModel;
    DatasetTableModel materialDataModel;

    QCPItemStraightLine *regressionLine;
    QVector<QCPAbstractItem *> regressionPoints;

    QJsonObject materialDataImport, peakListImport, spectralDataImport;

    QVector<int> fromJson(QJsonArray arr) {
        QVector<int> vec;
        for (int i = 0; i < arr.size(); i++) {
            vec.append(arr[i].toInt(-1));
        }

        return vec;
    }

    void save(QString path);
    void load(QString path);

private slots:
    void slotPlotterMouseMoved(QMouseEvent *ev);
    void slotPlotterMouseClicked(QMouseEvent *ev);
    void slotPeakListModelChanged(const QModelIndex, int, int);
    void slotMaterialDataModelChanged(const QModelIndex, int, int);
    void slotPeakListModelDataChanged(QModelIndex,QModelIndex,QVector<int>);
    void slotMaterialDataModelDataChanged(QModelIndex,QModelIndex,QVector<int>);
    void updateMaterialDataListDependents();
    void updatePeakListDependents();

    void on_loadGraphButton_clicked();
    void on_addPeakButton_clicked();
    void on_insertPeaksWithMouseButton_toggled(bool checked);
    void on_importPeakListButton_clicked();
    void on_removePeakButton_clicked();
    void on_materialDataAddButton_clicked();
    void on_materialDataRemoveButton_clicked();
    void on_materialDataImportButton_clicked();
    void on_regressionPointTable_cellChanged(int row, int column);
    void on_generateReportButton_clicked();
    void on_presetSaveButton_clicked();
    void on_nextTabButton_clicked();
    void on_previousTabButton_clicked();
    void on_workspaces_currentChanged(int index);
    void on_helpButton_clicked();
    void on_diagramExportButton_clicked();


    void on_exportPeakListButton_clicked();

    void on_graphHelpButton_clicked();

    void on_peakLineHideToggle_toggled(bool checked);

    void on_dataImportHelpButton_clicked();

    void on_graphHelpButton2_clicked();

    void on_spectralGraphResetPositionButton_clicked();

    void on_regressionResetPositionButton_clicked();

private:
    Ui::OESTemperatureCalculator *ui;
    QPen const redPen = QPen(Qt::red);
    QPen const blackPen = QPen(Qt::black);
    QPen const bluePen = QPen(Qt::blue);

    int performCalculation();
    int updateAreaComputationTable();
    void updateRegressionAndTemperature();
    void setPeakLinesVisible(bool visible);
};

#endif // OESTEMPERATURECALCULATOR_HPP
