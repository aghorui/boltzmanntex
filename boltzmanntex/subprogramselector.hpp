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

#ifndef SUBPROGRAMSELECTOR_HPP
#define SUBPROGRAMSELECTOR_HPP

#include <QWidget>
#include "oestemperaturecalculator/oestemperaturecalculator.hpp"
#include "simplegraphplot/simplegraphplot.hpp"

namespace Ui {
class SubprogramSelector;
};

class SubprogramSelector : public QWidget
{
    Q_OBJECT

signals:
    void done(int subprogramId);

public:
    explicit SubprogramSelector(QWidget *parent = nullptr);
    ~SubprogramSelector();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::SubprogramSelector *ui;
};

#endif // SUBPROGRAMSELECTOR_HPP
