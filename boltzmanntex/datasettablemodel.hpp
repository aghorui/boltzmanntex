/*
 * Dataset Table Model for QTableView
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

#ifndef DATASETTABLEMODEL_HPP
#define DATASETTABLEMODEL_HPP

#include <QAbstractTableModel>
#include <iostream>
#include "dataset.hpp"

class DatasetTableModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    Dataset *dataset = nullptr;

public:
    DatasetTableModel() {}
    DatasetTableModel(Dataset *dataset): dataset(dataset) {}

    int rowCount(const QModelIndex & = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int position, int rows, const QModelIndex &parent);
    bool insertRows(int position, int rows, const QModelIndex &parent);

    void clear();
    void setDataset(Dataset *set);

    bool appendRowVector(Dataset::Vector<double> row);
};

#endif // DATASETTABLEMODEL_HPP
