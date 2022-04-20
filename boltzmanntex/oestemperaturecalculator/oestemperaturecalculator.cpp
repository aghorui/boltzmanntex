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

#include "oestemperaturecalculator.hpp"
#include "ui_oestemperaturecalculator.h"

QString const separatorSelectItemData[] = { " ", "\t", ",", "@" };
QString const manualAnchors[] = {
    "oes:materialdata",
    "oes:spectraldata",
    "oes:peakareacomputation",
    "oes:regressionanalysis",
    "oes:resultanderror"
};

QString const materialDataColumnNames[] = {
    "Peak Wavelengths (Angs.)",
    "Transition Probability (s-1 * 10 ^ 8)",
    "Upper Level Energy (cm-1)",
    "Degeneracy (2 * J + 1)"
};

QString const peakListColumnNames[] = {
    "Peak Start (nm)",
    "Peak End (nm)",
};


OESTemperatureCalculator::OESTemperatureCalculator(QWidget *parent) :
    Subprogram(parent),
    ui(new Ui::OESTemperatureCalculator)
{
    ui->setupUi(this);
    ui->plotter->xAxis->ticker()->setTickCount(10);
    spectralPlot = ui->plotter->addGraph(); // Spectral Plot and lines
    regressionPlot = ui->regressionGraph->addGraph(); // Points
    fitLinePlot = ui->regressionGraph->addGraph(); // Fit line

    rangeStartLine = new QCPItemStraightLine(ui->plotter);
    rangeStartLine->point1->setCoords(0, 1);
    rangeStartLine->point2->setCoords(0, 0);
    rangeStartLine->setPen(redPen);
    rangeStartLine->setVisible(false);

    cursorLine = new QCPItemStraightLine(ui->plotter);
    cursorLine->point1->setCoords(0, 1);
    cursorLine->point2->setCoords(0, 0);

    connect(ui->plotter, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(slotPlotterMouseMoved(QMouseEvent*)));
    connect(ui->plotter, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(slotPlotterMouseClicked(QMouseEvent*)));

    materialData.addColumn(materialDataColumnNames[0]);
    materialData.addColumn(materialDataColumnNames[1]);
    materialData.addColumn(materialDataColumnNames[2]);
    materialData.addColumn(materialDataColumnNames[3]);
    materialDataModel.setDataset(&materialData);
    ui->materialDataTable->setModel(&materialDataModel);
    ui->materialDataTable->resizeColumnsToContents();

    peakList.addColumn(peakListColumnNames[0]);
    peakList.addColumn(peakListColumnNames[1]);
    peakListModel.setDataset(&peakList);
    ui->rangeList->setModel(&peakListModel);
    ui->rangeList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rangeList->verticalHeader()->setVisible(true);
    ui->rangeList->resizeColumnsToContents();

    ui->pointRegressionScreenSplitter->setSizes({ 3000, 1500 });

    ui->plotter->setInteraction(QCP::iRangeDrag, true);
    ui->plotter->setInteraction(QCP::iRangeZoom, true);
    ui->plotter->xAxis->setLabel("Wavelength (nm)");
    ui->plotter->yAxis->setLabel("Counts");
    ui->regressionGraph->setInteraction(QCP::iRangeDrag, true);
    ui->regressionGraph->setInteraction(QCP::iRangeZoom, true);
    ui->regressionGraph->xAxis->setLabel("Upper Energy Level (eV)");
    ui->regressionGraph->yAxis->setLabel("log(Iλ/gA)");

    connect(&peakListModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotPeakListModelChanged(QModelIndex,int,int)));
    connect(&peakListModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(slotPeakListModelChanged(QModelIndex,int,int)));
    connect(&peakListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(slotPeakListModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(&peakListModel, SIGNAL(modelReset()),
            this, SLOT(updatePeakListDependents()));

    connect(&materialDataModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotMaterialDataModelChanged(QModelIndex,int,int)));
    connect(&materialDataModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(slotMaterialDataModelChanged(QModelIndex,int,int)));
    connect(&materialDataModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(slotMaterialDataModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(&materialDataModel, SIGNAL(modelReset()),
            this, SLOT(updateMaterialDataListDependents()));

    ui->peakSelectionScreen->setEnabled(false);
    ui->peakAreaComputationScreen->setEnabled(false);
    ui->pointRegressionScreen->setEnabled(false);
    ui->resultScreen->setEnabled(false);

    ui->workspaces->setTabEnabled(1, false);
    ui->workspaces->setTabEnabled(2, false);
    ui->workspaces->setTabEnabled(3, false);
    ui->workspaces->setTabEnabled(4, false);
    on_workspaces_currentChanged(0);
}

OESTemperatureCalculator::~OESTemperatureCalculator()
{
    delete ui;
}

int OESTemperatureCalculator::updateAreaComputationTable()
{
    int ret = OESTools::integrate(spectrometerData[0].data,
                                  spectrometerData[1].data,
                                  peakList[0].data, peakList[1].data, integrationResult);

    if (ret < 0) {
        QMessageBox::critical(this, "Error", "Invalid Data");
        return -1;
    }

    ui->areaComputationTable->clearContents();
    ui->areaComputationTable->setRowCount(peakList.numRows());

    if (integrationResult.peakAreas.size() != peakList.numRows() ||
        integrationResult.noise.size() != peakList.numRows()) {
        qDebug() << "Illegal Call. Bug?";
        return -1;
    }

    qDebug() << peakList.numRows() << "Rows";
    for (int i = 0; i < peakList.numRows(); i++) {
        qDebug() << peakList[0][i] << peakList[1][i] << integrationResult.noise[i] << integrationResult.peakAreas[i];
        ui->areaComputationTable->setItem(i, 0,
            new QTableWidgetItem(QString::number(peakList[0][i]),
                                 QTableWidgetItem::Type));
        ui->areaComputationTable->setItem(i, 1,
            new QTableWidgetItem(QString::number(peakList[1][i]),
                                 QTableWidgetItem::Type));
        ui->areaComputationTable->setItem(i, 2,
            new QTableWidgetItem(QString::number(integrationResult.noise[i]),
                                 QTableWidgetItem::Type));
        ui->areaComputationTable->setItem(i, 3,
            new QTableWidgetItem(QString::number(integrationResult.peakAreas[i]),
                                 QTableWidgetItem::Type));
    }

    return 0;
}

void OESTemperatureCalculator::updateRegressionAndTemperature()
{
    Dataset::Column const &mPeakWavelengths = materialData[0];
    Dataset::Column const &mTransitionProbability = materialData[1];
    Dataset::Column const &mUpperLevelEnergy = materialData[2];
    Dataset::Column const &mDegeneracy = materialData[3];

    OESTools::regression(integrationResult.peakAreas,
                         mPeakWavelengths.data,
                         mTransitionProbability.data,
                         mUpperLevelEnergy.data,
                         mDegeneracy.data,
                         discardedPeaks,
                         regressionResult);

    ui->regressionPointTable->clearContents();
    ui->regressionPointTable->setRowCount(regressionResult.regressionPointX.size());
    ui->regressionPointTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    regressionLock = true;

    for (int i = 0; i < regressionResult.regressionPointX.size(); i++) {
        QTableWidgetItem *x = new QTableWidgetItem(
                    QString::number(regressionResult.regressionPointX[i]),
                    QTableWidgetItem::Type);
        x->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *y = new QTableWidgetItem(
                    QString::number(regressionResult.regressionPointY[i]),
                    QTableWidgetItem::Type);
        y->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *d = new QTableWidgetItem();
        qDebug() << i << discardedPeaks.size();
        if (i < discardedPeaks.size()) {
            d->setCheckState(discardedPeaks[i] ? Qt::Checked : Qt::Unchecked);
        }
        d->setTextAlignment(Qt::AlignCenter);
        ui->regressionPointTable->setItem(i, 0, x);
        ui->regressionPointTable->setItem(i, 1, y);
        ui->regressionPointTable->setItem(i, 2, d);
    }

    regressionPlot->setLineStyle(QCPGraph::lsNone);
    regressionPlot->setScatterStyle(QCPScatterStyle::ssCircle);
    regressionPlot->setData(regressionResult.regressionPointX, regressionResult.regressionPointY);
    fitLinePlot->setData(regressionResult.regressionPointX, regressionResult.regressionLineY);
    fitLinePlot->setPen(blackPen);
    ui->regressionGraph->replot();
    ui->regressionGraph->rescaleAxes();
    ui->regressionGraph->xAxis->scaleRange(2);
    ui->regressionGraph->yAxis->scaleRange(2);

    for (int i = 0; i < regressionLineLabels.size(); i++) {
        ui->regressionGraph->removeItem(regressionLineLabels[i]);
    }

    regressionLineLabels.clear();

    for (int i = 0; i < regressionResult.regressionPointX.size(); i++) {
        QCPItemText *label = new QCPItemText(ui->regressionGraph);
        regressionLineLabels.push_back(label);
        label->setPositionAlignment(Qt::AlignBottom | Qt::AlignLeft);
        label->position->setType(QCPItemPosition::ptPlotCoords);
        label->position->setCoords(regressionResult.regressionPointX[i],
                                   regressionResult.regressionPointY[i]); // place position at center/top of axis rect
        label->setText(QString::number(i+1));
        if (discardedPeaks[i] == true) {
            label->setColor(Qt::red);
        }
        label->setVisible(true);
    }

    regressionLock = false;

    ui->resultBox2->setText(QString::number(regressionResult.temperature));
    ui->resultBox->setText(QString::number(regressionResult.temperature));
    double correlation = OESTools::correlation(regressionResult, discardedPeaks);

    ui->correlationCoefficient->setText(QString::number(correlation));
    ui->determinationCoefficient->setText(QString::number(correlation * correlation));
    ui->determinationCoefficient2->setText(QString::number(correlation * correlation));
    setSaveStatus(false);
    on_workspaces_currentChanged(-1);
    ui->regressionGraph->replot();
}

int OESTemperatureCalculator::performCalculation()
{
    if (updateAreaComputationTable() < 0) {
        return -1;
    }

    updateRegressionAndTemperature();

    return 0;
}

void OESTemperatureCalculator::updateMaterialDataListDependents()
{
    discardedPeaks.resize(materialData.numRows());
    ui->presetSaveButton->setEnabled(materialData.numRows() > 0);
    ui->peakSelectionScreen ->setEnabled(materialData.numRows() > 0);
    ui->workspaces->setTabEnabled(1, materialData.numRows() > 0);
    ui->spectrumPeakCountLabel->setText(QString::number(materialData.numRows()));
    on_workspaces_currentChanged(ui->workspaces->currentIndex());
    setSaveStatus(false);
}

void OESTemperatureCalculator::save(QString path)
{
    QJsonArray discardedPeaksJson;

    for (int i = 0; i < discardedPeaks.size(); i++) {
        if (discardedPeaks[i] == true) {
            discardedPeaksJson.append(i);
        }
    }

    QJsonObject doc {
        { "header", SUBPROGRAM_JSON_HEADER },
        { "materialDataImport", materialDataImport },
        { "peakListImport", peakListImport },
        { "spectralDataImport", spectralDataImport },
        { "discardedPeaks",   discardedPeaksJson }
    };

    QFile saveFile(path);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Couldn't open " << path;
        throw std::invalid_argument("Could not open file.");
    }

    saveFile.write(QJsonDocument(doc).toJson());
    saveFile.close();
    setSaveStatus(true);
}

void OESTemperatureCalculator::load(QString path)
{
    QFile loadFile(path);
    bool error = false;

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open " << path;
        throw std::invalid_argument("Could not open file.");
    }

    QJsonDocument data = QJsonDocument::fromJson(loadFile.readAll());
    qDebug() << "Data" << data.object().keys();
    if (data["materialDataImport"].isObject()) {
        materialData.loadAndReplace(
                    data["materialDataImport"]["path"].toString(),
                    QRegExp(data["materialDataImport"]["separator"].toString()),
                    fromJson(data["materialDataImport"]["colMap"].toArray()));
        materialData.setHeaderNames({
             materialDataColumnNames[0],
             materialDataColumnNames[1],
             materialDataColumnNames[2],
             materialDataColumnNames[3]
         });
        materialDataModel.setDataset(&materialData);
    } else {
        error = true;
    }

    if (data["spectralDataImport"].isObject()) {
        spectrometerData.loadAndReplace(
                    data["spectralDataImport"]["path"].toString(),
                    QRegExp(data["spectralDataImport"]["separator"].toString()),
                    fromJson(data["spectralDataImport"]["colMap"].toArray()));
        spectralPlot->setData(spectrometerData[0].data, spectrometerData[1].data);
        ui->plotter->replot();
        ui->plotter->rescaleAxes();
        ui->inputFile->setText(data["spectralDataImport"]["path"].toString());

        spectrometerDataLoaded = true;
        spectrometerData.sort(0);
    } else {
        error = true;
    }

    if (data["peakListImport"].isObject()) {
        peakList.loadAndReplace(
                    data["peakListImport"]["path"].toString(),
                    QRegExp(data["peakListImport"]["separator"].toString()),
                    fromJson(data["peakListImport"]["colMap"].toArray()));
        peakList.setHeaderNames({
            peakListColumnNames[0],
            peakListColumnNames[1]
        });
        peakListModel.setDataset(&peakList);
    } else {
        error = true;
    }

    if (data["discardedPeaks"].isArray()) {
        QJsonArray arr = data["discardedPeaks"].toArray();
        discardedPeaks.fill(false);
        for (int i = 0; i < arr.size(); i++) {
            discardedPeaks[arr[i].toInt()] = true;
        }
//        for (int i = 0; i < arr.size(); i++) {
//            ui->regressionPointTable->item(arr[i].toInt(), 2)->setCheckState(Qt::Checked);
//        }
    } else {
        error = true;
    }

    updateMaterialDataListDependents();
    updatePeakListDependents();
    performCalculation();
    updateRegressionAndTemperature();

    if (error) {
        QMessageBox::warning(this, "Warning", "Errors were encountered in loading file.");
    }
}

void OESTemperatureCalculator::slotPlotterMouseMoved(QMouseEvent *ev)
{
    double x = ui->plotter->xAxis->pixelToCoord(ev->localPos().x());
    cursorLine->point1->setCoords(x, 0.0);
    cursorLine->point2->setCoords(x, 1.0);
    ui->plotter->replot();
}

void OESTemperatureCalculator::slotPlotterMouseClicked(QMouseEvent *ev)
{
    if (!editRange)
        return;
    double x = ui->plotter->xAxis->pixelToCoord(ev->localPos().x());
    rangeStartLine->point1->setCoords(x, 0.0);
    rangeStartLine->point2->setCoords(x, 1.0);
    rangeStartLine->setVisible(true);

    if (!rangeStarted) {
        rangeStarted = true;
        rangeStart = x;
    } else { // Add range to list
        QString out;
        QTextStream t(&out);
        if (x > rangeStart)
           t << "(" << rangeStart << ", " << x << ")";
        else
            t << "(" << x << ", " << rangeStart << ")";
        //model.setData(q, rangeStart);
        qDebug() << peakListModel.appendRowVector({ rangeStart, x });
        rangeStarted = false;
        rangeStartLine->setVisible(false);
    }
    ui->plotter->replot();
}

void OESTemperatureCalculator::slotPeakListModelChanged(const QModelIndex, int, int)
{
    //qDebug() << "Data Changed";
    updatePeakListDependents();
}

void OESTemperatureCalculator::slotMaterialDataModelChanged(const QModelIndex, int, int)
{
    //qDebug() << materialData.numRows();
    updateMaterialDataListDependents();
}

void OESTemperatureCalculator::slotPeakListModelDataChanged(QModelIndex,QModelIndex,QVector<int>)
{
    updatePeakListDependents();
}

void OESTemperatureCalculator::slotMaterialDataModelDataChanged(QModelIndex,QModelIndex,QVector<int>)
{
    updateMaterialDataListDependents();
}

void OESTemperatureCalculator::on_addPeakButton_clicked()
{
    (void) peakListModel.appendRowVector({ 0, 0 });
}

void OESTemperatureCalculator::on_insertPeaksWithMouseButton_toggled(bool checked)
{
    editRange = checked;
}

void OESTemperatureCalculator::on_importPeakListButton_clicked()
{
    CSVImporter csv;

    csv.setDatasetInputFields({
        peakListColumnNames[0],
        peakListColumnNames[1]
    });
    int ret = csv.exec();
    if (ret == QDialog::Rejected) {
        return;
    }

    peakList = csv.getDataset();
    peakListImport = csv.getJsonImportParameters();
    peakListModel.setDataset(&peakList);
}

void OESTemperatureCalculator::on_removePeakButton_clicked()
{
    QModelIndexList list =  ui->rangeList->selectionModel()->selectedIndexes();
    if (list.empty()) {
        return;
    }
    peakListModel.removeRow(list[0].row());
}

void OESTemperatureCalculator::setPeakLinesVisible(bool visible)
{
    if (visible == peakLinesVisible)
        return;

    for (int i = 0; i < lines.size(); i++) {
        lines[i]->setVisible(visible);
    }

    peakLinesVisible = visible;
    ui->plotter->replot();
}

void OESTemperatureCalculator::updatePeakListDependents()
{
    bool flag = (materialData.numRows() == peakList.numRows()) && spectrometerDataLoaded;

    for (int i = 0; i < lines.size(); i++) {
        ui->plotter->removeItem(lines[i]);
    }

    lines.clear();

    for (int i = 0; i < peakList.numRows(); i++) {
        QCPItemStraightLine *newLine = new QCPItemStraightLine(ui->plotter);
        newLine->point1->setCoords(peakList[0][i], 0.0);
        newLine->point2->setCoords(peakList[0][i], 1.0);
        newLine->setPen(redPen);

        QCPItemStraightLine *newLine2 = new QCPItemStraightLine(ui->plotter);
        newLine2->point1->setCoords(peakList[1][i], 0.0);
        newLine2->point2->setCoords(peakList[1][i], 1.0);
        newLine2->setPen(redPen);
        lines.push_back(newLine);
        lines.push_back(newLine2);
    }

    if (flag) {
        if (performCalculation() < 0) {
            return;
        }
    }

    ui->peakAreaComputationScreen->setEnabled(flag);
    ui->resultScreen->setEnabled(flag);
    ui->pointRegressionScreen->setEnabled(flag);
    ui->workspaces->setTabEnabled(2, flag);
    ui->workspaces->setTabEnabled(3, flag);
    ui->workspaces->setTabEnabled(4, flag);
    on_workspaces_currentChanged(ui->workspaces->currentIndex());

    ui->plotter->replot();

    setSaveStatus(false);
}

void OESTemperatureCalculator::on_materialDataAddButton_clicked()
{
    materialDataModel.insertRow(materialDataModel.rowCount());
}

void OESTemperatureCalculator::on_materialDataRemoveButton_clicked()
{
    QModelIndexList list =  ui->materialDataTable->selectionModel()->selectedIndexes();
    if (list.empty()) {
        return;
    }
    materialDataModel.removeRow(list[0].row());
}

void OESTemperatureCalculator::on_materialDataImportButton_clicked()
{
    CSVImporter csv;

    csv.setDatasetInputFields({
        materialDataColumnNames[0],
        materialDataColumnNames[1],
        materialDataColumnNames[2],
        materialDataColumnNames[3]
    });
    int ret = csv.exec();
    if (ret == QDialog::Rejected) {
        return;
    }

    materialData = csv.getDataset();
    materialDataImport = csv.getJsonImportParameters();

    materialDataModel.setDataset(&materialData);
    updateMaterialDataListDependents();
}

void OESTemperatureCalculator::on_loadGraphButton_clicked()
{
    CSVImporter csv;

    csv.setDatasetInputFields({ "Wavelength (nm)", "Counts" });
    int ret = csv.exec();
    if (ret == QDialog::Rejected) {
        return;
    }

    spectrometerData = csv.getDataset();
    spectralDataImport = csv.getJsonImportParameters();

    spectralPlot->setData(spectrometerData[0].data, spectrometerData[1].data);
    ui->plotter->replot();
    ui->plotter->rescaleAxes();
    ui->inputFile->setText(csv.getCurrentPath());

    spectrometerDataLoaded = true;
    spectrometerData.sort(0);

    bool flag = (materialData.numRows() == peakList.numRows()) && spectrometerDataLoaded;

    if (flag) {
        performCalculation();
        ui->peakAreaComputationScreen->setEnabled(flag);
        ui->resultScreen->setEnabled(flag);
        ui->pointRegressionScreen->setEnabled(flag);
    }

    updatePeakListDependents();
}

void OESTemperatureCalculator::on_regressionPointTable_cellChanged(int row, int column)
{
    if (regressionLock)
        return;

    if (column != 2) {
        return;
    }

    QVariant value = ui->regressionPointTable->item(row, column)->data(Qt::CheckStateRole);

    bool ok;
    int val = value.toInt(&ok);

    if (!ok) {
        qDebug() << "Could not get column state";
        return;
    }

    if (val == Qt::Checked) {
        discardedPeaks[row] = true;
        regressionLineLabels[row]->setColor(Qt::red);
    } else {
        discardedPeaks[row] = false;
        regressionLineLabels[row]->setColor(Qt::black);
    }

    qDebug() << val;
    updateRegressionAndTemperature();
}

void OESTemperatureCalculator::on_generateReportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", "", "PDF Files (*.pdf);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
    }

    QTextDocument doc;
    QPrinter printer(QPrinter::ScreenResolution);

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(fileName);

    QFile styleSheet(":/html/report.css");

    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qDebug() << "failed to load stylesheet";
        return;
    }

    doc.setDefaultStyleSheet(styleSheet.readAll());
    ReportGenerator r(doc, printer);

    r.insertReportHeader("OES Thermal Plasma Temperature Calculation");
    r.insertHeading(2, "Material Data");
    r.insertTableFromDataset(materialData);
    r.insertHeading(2, "Peaks");
    r.insertTableFromDataset(peakList);

    setPeakLinesVisible(false);

    ui->plotter->xAxis->ticker()->setTickCount(10);
    cursorLine->setVisible(false);
    QPixmap spectralDataPlot = ui->plotter->toPixmap(printer.pageRect(QPrinter::DevicePixel).width(), 500);
    (void) r.insertPixmap(spectralDataPlot, "spectralDataPlot");
    setPeakLinesVisible(peakLinesVisible);
    cursorLine->setVisible(true);

    r.insertHeading(2, "Peak Area Calculation");
    r.insertTableFromWidget(*ui->areaComputationTable);

    r.insertHeading(2, "Regression Calculation");
    r.insertTableFromWidget(*ui->regressionPointTable, true);
    QPixmap regressionPlot = ui->regressionGraph->toPixmap(printer.pageRect(QPrinter::DevicePixel).width(), 500);
    (void) r.insertPixmap(regressionPlot, "regressionPlot");

    r.doc << "(Discarded points are marked in red)";
    r.insertHeading(2, "Final Temperature");
    r.doc << "<b>Final Calculated Temperature:</b> "
          << regressionResult.temperature
          << " K <br />";
    r.doc << "<b>Correlation Coefficient (R):</b> "
          << ui->correlationCoefficient->text()
          << "<br />";
    r.doc << "<b>Coefficient of Determination (R<sup>2</sup>):</b> "
          << ui->determinationCoefficient->text()
          << "<br />";

    doc.setHtml(r.docData);
    doc.setPageSize(printer.pageRect().size());
    doc.print(&printer);
}

void OESTemperatureCalculator::on_presetSaveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV Data", "", "All Files (*);;Data files (*.dat *.csv *.data *.raw)");
    if (fileName.isEmpty()) {
        return;
    }
    materialData.save(fileName);
}

void OESTemperatureCalculator::on_nextTabButton_clicked()
{
    ui->workspaces->setCurrentIndex(ui->workspaces->currentIndex() + 1);
}

void OESTemperatureCalculator::on_previousTabButton_clicked()
{
    ui->workspaces->setCurrentIndex(ui->workspaces->currentIndex() - 1);
}

void OESTemperatureCalculator::on_workspaces_currentChanged(int index)
{
    // index + 1 does not go out of bounds because of evaluation order
    ui->nextTabButton->setEnabled(index < ui->workspaces->count() - 1 && ui->workspaces->isTabEnabled(index + 1));
    ui->previousTabButton->setEnabled(index >= 0 && ui->workspaces->isTabEnabled(index - 1));
}

void OESTemperatureCalculator::on_helpButton_clicked()
{
    emit manualRequest(manualAnchors[ui->workspaces->currentIndex()]);
}

void OESTemperatureCalculator::on_diagramExportButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select a Directory to Save Graphs In");

    if (directory.isEmpty()) {
        qDebug() << "a";
        return;
    }

    ui->plotter->savePng(directory + "/" + "BoltzmannTex SpectralDataPlot " + QDateTime::currentDateTime().toString(Qt::TextDate) + ".png", 1280, 720);
    ui->regressionGraph->savePng(directory + "/" + "BoltzmannTex RegressionPlot (" + QDateTime::currentDateTime().toString(Qt::TextDate) + ").png", 1280, 720);
    qDebug() << "c";
}


void OESTemperatureCalculator::on_exportPeakListButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV Data", "", "All Files (*);;Data files (*.dat *.csv *.data *.raw)");
    if (fileName.isEmpty()) {
        return;
    }
    peakList.save(fileName);
}

void OESTemperatureCalculator::on_graphHelpButton_clicked()
{
    emit manualRequest("graphs");
}

void OESTemperatureCalculator::on_peakLineHideToggle_toggled(bool checked)
{
    setPeakLinesVisible(checked);
}

void OESTemperatureCalculator::on_graphHelpButton_2_clicked()
{
    emit manualRequest("graphs");
}

void OESTemperatureCalculator::on_dataImportHelpButton_clicked()
{
    emit manualRequest("oes:materialdataimport");
}
