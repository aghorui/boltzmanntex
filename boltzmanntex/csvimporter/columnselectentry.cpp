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

#include "columnselectentry.hpp"
#include "ui_columnselectentry.h"

ColumnSelectEntry::ColumnSelectEntry(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ColumnSelectEntry)
{
    ui->setupUi(this);
}

void ColumnSelectEntry::setName(QString name)
{
    ui->name->setText(name);
}

QString ColumnSelectEntry::getName()
{
    return ui->name->text();
}


void ColumnSelectEntry::setOptions(QStringList &options)
{
    ui->options->addItems(options);
}

int ColumnSelectEntry::getSelectedOption()
{
    return ui->options->currentIndex();
}

void ColumnSelectEntry::setModel(QStringListModel &model)
{
    ui->options->setModel(&model);
}

ColumnSelectEntry::~ColumnSelectEntry()
{
    delete ui;
}
