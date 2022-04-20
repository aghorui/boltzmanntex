/*
 * Disclaimer Dialog Window for Program
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

#ifndef DISCLAIMERDIALOG_HPP
#define DISCLAIMERDIALOG_HPP

#include <QDialog>

namespace Ui {
class DisclaimerDialog;
}

class DisclaimerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisclaimerDialog(QWidget *parent = nullptr);
    ~DisclaimerDialog();

private:
    Ui::DisclaimerDialog *ui;
};

#endif // DISCLAIMERDIALOG_HPP
