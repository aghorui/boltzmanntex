/*
 * Subprogram Selection Widget
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


#include "subprogramselector.hpp"
#include "ui_subprogramselector.h"

SubprogramSelector::SubprogramSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubprogramSelector)
{
    ui->setupUi(this);
    ui->versionLabel->setText(QString("Version ") + BOLTZMANNTEX_VERSON " ("
                              BOLTZMANNTEX_BUILD_NUMBER ", " BOLTZMANNTEX_BUILD_COMMIT_DATE ")");
}

SubprogramSelector::~SubprogramSelector()
{
    delete ui;
}

void SubprogramSelector::on_pushButton_clicked()
{
    emit done(OESTemperatureCalculator::subprogramId);
}

void SubprogramSelector::on_pushButton_2_clicked()
{
    emit done(SimpleGraphPlot::subprogramId);
}
