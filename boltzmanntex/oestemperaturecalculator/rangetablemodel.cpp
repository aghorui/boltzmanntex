/*
 * Range Table Model for QTableView (unused)
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

#include "rangetablemodel.hpp"

int RangeTableModel::rowCount(const QModelIndex &) const
{
    return list.size();
}

int RangeTableModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant RangeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole)) {
            return QVariant();
    }

    if (index.column() > 1 || index.column() < 0 ||
        index.row() >= list.size() || index.row() < 0)
        return QVariant();


    if (index.column() == 0)
        return QVariant(list[index.row()].left);
    else
        return QVariant(list[index.row()].right);

}

bool RangeTableModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    qDebug() << "run" << index.row() << index.column() << index.isValid() << role;

    if (!index.isValid() || role != Qt::EditRole) {
            return false;
    }

    if (index.column() > 1 || index.column() < 0 ||
        index.row() >= list.size() || index.row() < 0) {
        qDebug() << "<< 2";
        return false;
    }

    qDebug() << index.row() << index.column();

    bool ok;
    float floatval = value.toFloat(&ok);
    if (!ok)
        return false;
    if (index.column() == 0)
        list[index.row()].left = floatval;
    else
        list[index.row()].right = floatval;

    emit dataChanged(index, index);
    return true;
}

QVariant RangeTableModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QVariant("Start");
        }
        if (section == 1) {
            return QVariant("End");
        }
    }

    return QVariant();
}

Qt::ItemFlags RangeTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool RangeTableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    if (position > list.size() || position < 0)
        return false;
    beginInsertRows(parent, position, position + rows - 1);
    for (int i = 0; i < rows; i++) {
        list.insert(position, rows, {0, 0});
    }
    endInsertRows();
    return true;
}

bool RangeTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if (position >= list.size() || position < 0)
        return false;
    beginRemoveRows(parent, position, position + rows - 1);
    list.remove(position, rows);
    endRemoveRows();
    return true;
}

bool RangeTableModel::set(int row, float l, float r)
{
    if (row >= list.size() || row < 0)
        return false;

    list[row].left = l;
    list[row].right = r;
    return true;
}

bool RangeTableModel::insert(float l, float r)
{
    if (!insertRow(rowCount()))
        return false;
    setData(index(rowCount() - 1, 0), l);
    setData(index(rowCount() - 1, 1), r);
    for (int i = 0; i < list.size(); i++) {
        qDebug() << "{" << list[i].left << list[i].right << "}";
    }
    return true;
}
