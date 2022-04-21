/*
 * Column Entry Selector Widget for CSVImporter
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

#ifndef COLUMNSELECTENTRY_HPP
#define COLUMNSELECTENTRY_HPP

#include <QFrame>
#include <QStringListModel>

namespace Ui {
class ColumnSelectEntry;
}

class ColumnSelectEntry : public QFrame
{
    Q_OBJECT

public:
    explicit ColumnSelectEntry(QWidget *parent = nullptr);
    void setName(QString name);
    QString getName();

    void setOptions(QStringList &options);
    void setModel(QStringListModel &model);
    void setCurrentIndex(int index);
    int getSelectedOption();
    ~ColumnSelectEntry();

private:
    Ui::ColumnSelectEntry *ui;
};

#endif // COLUMNSELECTENTRY_HPP
