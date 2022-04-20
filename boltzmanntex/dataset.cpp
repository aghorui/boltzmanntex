/*
 * Datastructure for Storing and Accessing Datasets
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

#include "dataset.hpp"

void Dataset::addRow()
{
    for (auto &i : data) {
        i.data.push_back(0);
    }
}

void Dataset::addRow(Index pos)
{
    for (auto &i : data) {
        i.data.insert(i.data.begin() + pos, 0);
    }
}

void Dataset::addRow(Vector<CellData> const& row)
{
    if (row.size() < numColumns()) {
        throw std::invalid_argument("Row is smaller than number of columns.");
    }

    for (Index i = 0; i < numColumns(); i++) {
        data[i].data.push_back(row[i]);
    }
}

void Dataset::addRow(Vector<CellData> const& row, Index pos)
{
    if (row.size() < numColumns()) {
        throw std::invalid_argument("Row is smaller than number of columns.");
    }

    for (Dataset::Index i = 0; i < numColumns(); i++) {
        data[i].data.insert(data[i].data.begin() + pos, row[i]);
    }
}

void Dataset::addColumn(String name)
{
    data.push_back(Column(name));
}


void Dataset::insertRows(Index pos, Index count)
{
    for (auto &i : data) {
        (void) i.data.insert(i.data.begin() + pos, count, 0);
    }
}

void Dataset::removeRows(Index pos, Index count)
{
    for (auto &i : data) {
        (void) i.data.remove(pos, count);
    }
}

void Dataset::clear()
{
    loaded = false;
    data.clear();
}

bool Dataset::indexcmp(IndexedCellData a, IndexedCellData b)
{
    return a.value < b.value;
}

// We don't have any direct method of sorting the columns, so we first sort
// the primary column and build an index.
// Inefficient. Probably doable in O(1) space if we have a custom container,
// but this will be used very rarely, and implementing it would increase
// complexity by a lot. Another simpler method could be just implementing a
// custom sort function, but would people like that?
int Dataset::sort(Index column)
{
    IndexedCellData tmp;
    QVector<CellData> tmpvalues(data.size());

    if (column > data.size() || column < 0) {
        return -1;
    }

    QVector<IndexedCellData> index(data[column].data.size());

    for (int i = 0; i < data[column].data.size(); i++) {
        index[i].value = data[column][i];
        index[i].order = i;
    }

    std::sort(index.begin(), index.end(), indexcmp);

    // Now sort all columns based on generated index.
    for (int i = 0; i < index.size(); i++) {
        while (index[i].order != i)
        {
            for (int j = 0; j < data.size(); j++) {
                tmpvalues[j] = data[j][index[i].order];
            }

            tmp = index[index[i].order];

            for (int j = 0; j < data.size(); j++) {
                data[j][index[i].order] = data[j][i];
            }

            index[index[i].order] = index[i];


            for (int j = 0; j < data.size(); j++) {
                data[j][i] = tmpvalues[j];
            }

            index[i] = tmp;
        }
    }

    return 0;
}

void Dataset::save(String path)
{
    QFile file(path);
    QTextStream stream(&file);

    if (!file.open(QIODevice::WriteOnly)) {
        throw std::invalid_argument("Could not open file");
    }

    for (int i = 0; i < numRows(); i++) {

        for (int j = 0; j < numColumns() - 1; j++) {
            stream << data[j][i] << " ";
        }
        stream << data[numColumns() - 1][i] << "\n";
    }

    file.close();
}

QString Dataset::toString()
{
    QString ret;
    QTextStream stream(&ret);

    for (int i = 0; i < numRows(); i++) {

        for (int j = 0; j < numColumns() - 1; j++) {
            stream << data[j][i] << " ";
        }
        stream << data[numColumns() - 1][i] << "\n";
    }

    return ret;
}

void Dataset::load(String path, QRegExp separator, QVector<Index> remap,
                   bool firstLineIsHeader, Index maxRows)
{
    Index predictedNumColumns = 0;
    File file(path);
    Index rowCount = 0;

    if (!loaded) {
        loaded = true;
    } else {
        throw std::runtime_error("(Internal Error) Dataset already loaded");
    }

    if (file.open(QIODevice::ReadOnly) == false) {
        qDebug() << "file failed to open";
        throw std::invalid_argument("Could not open file.");
    }

    QString line = file.readLine();
    QStringList k = line.split(separator, QString::SkipEmptyParts);
    predictedNumColumns = k.size();

    if (firstLineIsHeader) {
        for (Index i = 0; i < predictedNumColumns; i++) {
            addColumn(k[i]);
        }
    } else {
        for (Index i = 0; i < predictedNumColumns; i++) {
            addColumn("col" + QString::number(i + 1));
            data[data.size() - 1].data.push_back(k[i].toDouble());
        }
    }

    // TODO manage nonunique column assignments
    if (remap.size() > 0) {
        if (data.size() != remap.size()) {
            qDebug() << "file failed to open";
            throw std::invalid_argument("Bug: Incorrect column mapping supplied.");
        }
    } else {
        for (int i = 0; i < data.size(); i++) {
            remap.push_back(i);
        }
    }

    while (!file.atEnd()) {
        line = file.readLine();
        k = line.split(separator, QString::SkipEmptyParts);
        if (k.size() != predictedNumColumns) {
            qDebug() << "failed to load data from file";
            throw std::invalid_argument("Invalid data");
        }

        for (Index i = 0; i < data.size(); i++) {
            data[remap[i]].data.push_back(k[i].toDouble());
        }

        if (maxRows > 0) {
            rowCount++;
            if (rowCount == maxRows) {
                break;
            }
        }
    }
}

void Dataset::loadString(QString csvdata, QVector<Index> remap)
{
    QString const separator = " ";
    Index predictedNumColumns = 0;
    Index rowCount = 0;

    if (!loaded) {
        loaded = true;
    } else {
        throw std::runtime_error("(Internal Error) Dataset already loaded");
    }

    QTextStream file(&csvdata);
    QString line = file.readLine();
    QStringList k = line.split(separator, QString::SkipEmptyParts);
    predictedNumColumns = k.size();

    for (Index i = 0; i < predictedNumColumns; i++) {
        addColumn("col" + QString::number(i + 1));
        data[data.size() - 1].data.push_back(k[i].toDouble());
    }

    // TODO manage nonunique column assignments
    if (remap.size() > 0) {
        if (data.size() != remap.size()) {
            qDebug() << "file failed to open";
            throw std::invalid_argument("Bug: Incorrect column mapping supplied.");
        }
    } else {
        for (int i = 0; i < data.size(); i++) {
            remap.push_back(i);
        }
    }

    while (!file.atEnd()) {
        line = file.readLine();
        k = line.split(separator, QString::SkipEmptyParts);
        if (k.size() != predictedNumColumns) {
            qDebug() << "failed to load data from file";
            throw std::invalid_argument("Invalid data");
        }

        for (Index i = 0; i < data.size(); i++) {
            data[remap[i]].data.push_back(k[i].toDouble());
        }
    }
}

void Dataset::loadAndReplace(String path, QRegExp separator, QVector<Index> remap,
                    bool firstLineIsHeader, Index maxRows)
{
    clear();
    load(path, separator, remap, firstLineIsHeader, maxRows);
}
