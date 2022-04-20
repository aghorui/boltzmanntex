/*
 * Abstract Base Class for Subprograms
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


#ifndef SUBPROGRAM_HPP
#define SUBPROGRAM_HPP

#include "version.hpp"
#include <QJsonObject>
#include <QString>
#include <QFrame>
#include <QDir>
#include <QFileDialog>

#define SUBPROGRAM_JSON_HEADER \
    QJsonObject { \
        { "boltzmannTexVersion", BOLTZMANNTEX_VERSON }, \
        { "subprogram", subprogramId }, \
        { "subprogramVersion", subprogramVersion } \
    }

class Subprogram: public QFrame
{
    Q_OBJECT

protected:
    bool saveStatus = true; // True when program opens a blank subprogram

public:
    static int const subprogramId = -1;
    static constexpr const char * const subprogramVersion = "";

    explicit Subprogram(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()): QFrame(parent, f) {}
    bool isSaved() { return saveStatus; }

    // TODO make these functions virtual
    virtual void save(QString) = 0;
    virtual void load(QString) = 0;

    virtual void setSaveStatus(bool status);

signals:
    void saveStatusUpdated(bool status);
    void manualRequest(QString anchor);
};

#endif // SUBPROGRAM_HPP
