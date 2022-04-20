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


#ifndef CSVIMPORTER_HPP
#define CSVIMPORTER_HPP

#include <QMap>
#include <QDialog>
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "columnselectentry.hpp"
#include "datasettablemodel.hpp"
#include "settings.hpp"
#include "dataset.hpp"

namespace Ui {
class CSVImporter;
}

class CSVImporter : public QDialog
{

    Q_OBJECT

public:
    explicit CSVImporter(QWidget *parent = nullptr);
    ~CSVImporter();

    void setDatasetInputFields(QStringList inputList);
    Dataset&& getDataset();
    QVector<int>&& getColumnMapping();
    QJsonObject getJsonImportParameters();
    QJsonObject getDataAsJson();

    QString getCurrentPath() { return currentPath; }

private slots:
    void on_loadButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_refreshButton_clicked();

    void on_dataSeperatorSelect_currentIndexChanged(int index);

private:
    QVector<int> userColumnMap;
    Ui::CSVImporter *ui;
    DatasetTableModel tableModel;
    bool initial = true;
    QStringListModel model;
    QVector<ColumnSelectEntry *> entryList;
    Dataset d;
    QString currentPath;
    QRegExp separator;
};

#endif // CSVIMPORTER_HPP
