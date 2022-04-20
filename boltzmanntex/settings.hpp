/*
 * Persistent Program Configuration Storage
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


#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "version.hpp"
#include <QSettings>
#include <QString>
#include <QFileInfo>
#include <QDir>

namespace boltzmanntex {

// TODO persistent storage
//class Settings {
//private:
//    QSettings settings;

//public:
//    void save();
//    void load();

//    }

//    QString getlastVisitedDirectory() {  }
//    void setLastVisitedDirectory() {  }

//    static QSettings &&getSettings(QObject *parent = nullptr) {
//    }

//};

extern QString lastVisitedDirectory;

void setLastVisitedDirectory(QString path);

}

#endif // SETTINGS_HPP
