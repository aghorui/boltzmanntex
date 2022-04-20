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

#ifndef DATASET_HPP
#define DATASET_HPP

#include <exception>
#include <algorithm>
#include <QDebug>
#include <QMap>
#include <QVector>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStringList>

struct Dataset {
    using File = QFile;
    using Index = int;
    using CellData = double;
    using String = QString;
    template<typename T> using Vector = QVector<T>;
    template<typename P, typename Q> using Map = QMap<P, Q>;

    struct Column {
        template<typename T> using Vector = QVector<T>;
        template<typename P, typename Q> using Map = QMap<P, Q>;

        String name;
        Vector<CellData> data;
        Column() {}
        Column(String name, Vector<CellData> column_data = {}):
            name(name), data(column_data) {}
        CellData& operator[](int i) { return Column::data[i]; }
    };

    struct IndexedCellData {
        double value;
        Index order;
    };


//protected:
    bool loaded = false;
    String rowSeparator = " ";
    String columnSeparator = "\n";
    String const defaultColumnSeparator = "\t";
    String const defaultRowSeparator = "\n";
    Vector<Index> sort_order;
    Vector<Column> data;

    Dataset() { qDebug() << data.size(); }

    Dataset(Dataset &&d):
        loaded(d.loaded),
        data(std::move(data)) { qDebug() << data.size(); }

    Dataset(String path, QRegExp separator = QRegExp("\\s+"), QVector<Index> remap = {},
            bool firstLineIsHeader = false, Index maxRows = 0) {
        qDebug() << data.size();
        load(path, separator, remap, firstLineIsHeader, maxRows);
    }

    Dataset &operator=(Dataset &&other)
    {
        if (this != &other) {
            data = std::move(other.data);
            rowSeparator = std::move(rowSeparator);
            columnSeparator = std::move(columnSeparator);
        }

        return *this;
    }

    CellData at(Index i, Index j) { return data[i][j]; } // TODO handle bound checking maybe

    Column& operator[](int i) {
        return data[i];
    }

    Index numColumns() const
    {
        return data.size();
    }

    Index numRows() const
    {
        if (numColumns() > 0)
            return data[0].data.size();
        else
            return 0;
    }

    QStringList getHeaderNames()
    {
        QStringList list;

        for (auto &i : data) {
            list.push_back(i.name);
        }

        return list;
    }

    String getRowSeparator() { return rowSeparator; }
    String getColumnSeparator() { return columnSeparator; }

    void addRow();
    void addRow(Index pos);
    void addRow(Vector<CellData> const& row);
    void addRow(Vector<CellData> const& row, Index pos);

    void addColumn(String name);

    void setHeaderNames(QStringList names)
    {
        for (int i = 0; i < data.size() && i < names.size(); i++)
        {
            data[i].name = names[i];
        }
    }

    void insertRows(Index pos, Index count);
    void removeRows(Index pos, Index count);

    static bool indexcmp(IndexedCellData a, IndexedCellData b);
    int sort(Index column);

    void clear();
    void save(String path);
    QString toString();

    void load(String path, QRegExp separator, QVector<Index> remap = {},
              bool firstLineIsHeader = false, Index maxRows = 0);

    void loadString(QString data, QVector<Index> remap);

    void loadAndReplace(String path, QRegExp separator, QVector<Index> remap = {},
                        bool firstLineIsHeader = false, Index maxRows = 0);

};

#endif // DATASET_HPP
