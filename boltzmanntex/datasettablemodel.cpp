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

#include "datasettablemodel.hpp"

int DatasetTableModel::rowCount(const QModelIndex &) const
{
    if (dataset != nullptr) {
        // qDebug() << "RowCount" << dataset->numRows();
        return dataset->numRows();
    } else {
        // qDebug() << "RowCount" << 0;
        return 0;
    }
}

int DatasetTableModel::columnCount(const QModelIndex &) const
{
    if (dataset != nullptr) {
        // qDebug() << "columnCount" << dataset->numColumns();
        return dataset->numColumns();
    } else {
        // qDebug() << "columnCount" << 0;
        return 0;
    }
}

QVariant DatasetTableModel::data(const QModelIndex &index, int role) const
{
    // qDebug() << "data" << index.row() << index.column() << index.isValid() << role;
    if (!index.isValid()  || (role != Qt::DisplayRole && role != Qt::EditRole)) {
        return QVariant();
    }

    if (dataset == nullptr) {
        return QVariant();
    } else {
        return QVariant((*dataset)[index.column()][index.row()]);
    }
}

QVariant DatasetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < 0 || section > columnCount()) {
            return QVariant();
        }
        if (dataset == nullptr) {
            return QVariant();
        } else {
            return QVariant((*dataset)[section].name);
        }
    }

    if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        if (section < 0 || section > rowCount()) {
           return QVariant();
        }
        return QVariant(QString::number(section + 1));
    }

    return QVariant();
}

bool DatasetTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // qDebug() << "setData" << index.row() << index.column() << index.isValid() << role;
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    bool ok;
    double actual = value.toDouble(&ok);

    if (!ok) {
        return false;
    }

    if (dataset != nullptr) {
        (*dataset)[index.column()][index.row()] = actual;
        emit dataChanged(index, index);
        return true;
    } else {
        return false;
    }
}

Qt::ItemFlags DatasetTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool DatasetTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if (position > (rowCount() - 1) || position < 0)
        return false;
    beginRemoveRows(parent, position, position + rows - 1);
    dataset->removeRows(position, rows);
    endRemoveRows();
    return true;
}

bool DatasetTableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    if (position > rowCount() || position < 0)
        return false;
    beginInsertRows(parent, position, position + rows - 1);
    dataset->insertRows(position, rows);
    endInsertRows();
    return true;
}

void DatasetTableModel::clear() {
    beginResetModel();
    dataset->clear();
    endResetModel();
}

void DatasetTableModel::setDataset(Dataset *set) {
    beginResetModel();
    dataset = set;
    endResetModel();
}

bool DatasetTableModel::appendRowVector(Dataset::Vector<double> row)
{
    if (row.size() < columnCount()) {
        return false;
    }

    insertRow(rowCount());

    for (int i = 0; i < columnCount(); i++) {
        // qDebug() << row << i;
        if (!setData(index(rowCount() - 1, i), row[i])) {
            removeRow(rowCount() - 1);
            return false;
        }
    }

    return true;
}
