/*
 * Simple Graph Plotter Subprogram
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

#ifndef SIMPLEGRAPHPLOT_HPP
#define SIMPLEGRAPHPLOT_HPP

#include "qcustomplot.h"
#include "subprogram.hpp"
#include "csvimporter/csvimporter.hpp"

namespace Ui {
class SimpleGraphPlot;
}

class SimpleGraphPlot : public Subprogram
{
    Q_OBJECT

public:
    static int const subprogramId = 1;
    static constexpr const char * const subprogramVersion = "0.1";

    explicit SimpleGraphPlot(QWidget *parent = nullptr);
    ~SimpleGraphPlot();
    Dataset currentDataset;

    void save(QString) {}
    void load(QString) {}
    void setSaveStatus(bool) {}

private slots:
    void on_applyButton_clicked();

    void on_loadGraphButton_clicked();

    void on_saveGraphButton_clicked();

private:
    Ui::SimpleGraphPlot *ui;
};

#endif // SIMPLEGRAPHPLOT_HPP
