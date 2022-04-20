/*
 * CSV File/Dataset Importer
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

#include "csvimporter.hpp"
#include "ui_csvimporter.h"

QString const separatorSelectItemData[] = { "\\s+", " +", "\\t+", ",", "@" };

CSVImporter::CSVImporter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVImporter)
{
    ui->setupUi(this);
    ui->previewTable->setModel(&tableModel);
    ui->customSeparatorInput->setVisible(false);
}

void CSVImporter::setDatasetInputFields(QStringList inputList)
{
    qDeleteAll(entryList);
    entryList.clear();

    for (int i = 0; i < inputList.size(); i++) {
        entryList.push_back(new ColumnSelectEntry());
        entryList.back()->setName(inputList[i]);
        entryList.back()->setModel(model);
        ui->assignmentList->layout()->addWidget(entryList.back());
    }
}

Dataset&& CSVImporter::getDataset()
{
    return std::move(d);
}

QVector<int>&& CSVImporter::getColumnMapping()
{
    return std::move(userColumnMap);
}

CSVImporter::~CSVImporter()
{
    qDeleteAll(entryList);
    delete ui;
}

void CSVImporter::on_loadButton_clicked()
{
    currentPath = QFileDialog::getOpenFileName(
                        this,
                        "Open Dataset",
                        boltzmanntex::lastVisitedDirectory,
                        "All Files (*);;Data files (*.dat *.csv *.data *.raw)");

    boltzmanntex::setLastVisitedDirectory(currentPath);

    if (currentPath.isEmpty())
        return;

    ui->filePath->setText(currentPath);

    separator = QRegExp(separatorSelectItemData[ui->dataSeperatorSelect->currentIndex()]);

    try {
        d.loadAndReplace(currentPath,
                         separator,
                         userColumnMap,
                         false, 5);
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error", e.what());
        ui->assignmentList->setEnabled(false);
    }
    ui->fileStatus->setText(QString("Loaded ") + QString::number(d.numColumns()) +
                            " columns, " + QString::number(d.numRows()) + "rows.");
    ui->assignmentList->setEnabled(true);
    model.setStringList(d.getHeaderNames());
    tableModel.setDataset(&d);
}

void CSVImporter::on_buttonBox_accepted()
{
    QVector<bool> columnSet(d.numColumns(), false);

    for (int i = 0; i < entryList.size(); i++) {
        int columnIndex = entryList[i]->getSelectedOption();
        if (columnIndex < 0) {
            QMessageBox::critical(this, "Error", "Please assign a unique column to all the requested data.");
            return;
        }
        if (columnSet[columnIndex] == true) {
            QMessageBox::critical(this, "Error", "Column assignments must be unique.");
            return;
        }
        columnSet[columnIndex] = true;
    }

    if (ui->dataSeperatorSelect->currentIndex() == ui->dataSeperatorSelect->count() - 1) {
        separator = QRegExp(ui->customSeparatorInput->text());
    } else {
        separator = QRegExp(separatorSelectItemData[ui->dataSeperatorSelect->currentIndex()]);
    }

    try {
        d.loadAndReplace(currentPath,
                         separator,
                         userColumnMap,
                         false);
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error", e.what());
        ui->assignmentList->setEnabled(false);
        return;
    }

    for (int i = 0; i < entryList.size(); i++) {
        int columnIndex = entryList[i]->getSelectedOption();
        d[columnIndex].name = entryList[i]->getName();
        userColumnMap.push_back(columnIndex);
    }

    done(QDialog::Accepted);
}

void CSVImporter::on_buttonBox_rejected()
{
    done(QDialog::Rejected);
}

QJsonArray toJson(QVector<int> vec) {
    QJsonArray arr;
    for (int i = 0; i < vec.size(); i++) {
        arr.append(vec[i]);
    }

    return arr;
}

QJsonObject CSVImporter::getJsonImportParameters() {
    return QJsonObject {
        { "path", currentPath },
        { "colMap", toJson(userColumnMap) },
        { "separator", separator.pattern() }
    };
}

QJsonObject CSVImporter::getDataAsJson() {
    return QJsonObject {
        { "colMap", toJson(userColumnMap) },
        { "separator", separator.pattern() },

    };
}

void CSVImporter::on_refreshButton_clicked()
{
    if (currentPath.isEmpty())
        return;

    ui->filePath->setText(currentPath);

    if (ui->dataSeperatorSelect->currentIndex() == ui->dataSeperatorSelect->count() - 1) {
        separator = QRegExp(ui->customSeparatorInput->text());
    } else {
        separator = QRegExp(separatorSelectItemData[ui->dataSeperatorSelect->currentIndex()]);
    }

    try {
        d.loadAndReplace(currentPath,
                         separator,
                         userColumnMap,
                         false, 5);
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error", e.what());
        ui->assignmentList->setEnabled(false);
    }

    ui->assignmentList->setEnabled(true);
    ui->fileStatus->setText(QString("Loaded ") + QString::number(d.numColumns()) +
                            " columns, " + QString::number(d.numRows()) + " rows.");
    model.setStringList(d.getHeaderNames());
    tableModel.setDataset(&d);
}

void CSVImporter::on_dataSeperatorSelect_currentIndexChanged(int index)
{
    ui->customSeparatorInput->setVisible(index == ui->dataSeperatorSelect->count() - 1);
    on_refreshButton_clicked();
}
