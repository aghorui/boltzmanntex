/*
 * OES Temperature Calculation Helper Functions
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

#ifndef OESTOOLS_HPP
#define OESTOOLS_HPP

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <QVector>
#include <QDebug>

class OESTools
{
public:
    using Index = int;
    using Data = double;
    using List = QVector<Data>;

    struct IntegrationResult {
        List noise;
        List peakAreas;
    };

    struct DataRange {
        int start;
        int end;
    };

    struct RegressionResult {
        List regressionPointX;
        List regressionPointY;
        List regressionLineY;
        Data temperature;
    };

private:
    static constexpr Data const HC = 6.6262e-34 * 3e8;
    static constexpr Data const K = 1.38e-23;

public:

    static inline DataRange find_range(List data, Data start, Data end)
    {
        Index a = std::lower_bound(data.begin(), data.end(), start,
                                   [](Data a, Data b){return a < b;}) - data.begin();

        Index b = std::upper_bound(data.rbegin(), data.rend(), end,
                                   [](Data a, Data b){return a >= b;}) - data.rbegin();
        b = data.size() - b - 1;
        return (DataRange) { a,  b };
//            start = std::lower_bound(data.begin(), data.end(), start,
//                                      [](Data a, Data b){return a < b;}),

//            end   = std::upper_bound(data.rbegin(), data.rend(), end,
//                                      [](Data a, Data b){return a >= b;})
    }

    static int integrate(
            List const &wavelength,
            List const &counts,
            List const &peakStart,
            List const &peakEnd,
            IntegrationResult &result);

    static int regression(
            List const &peakAreas,
            List const &spectralPeakWavelength,
            List const &transitionProbability,
            List const &upperLevelEnergy,
            List const &degeneracy,
            QVector<bool> const &discardedPeaks,
            RegressionResult &result);

    static float correlation(RegressionResult const &result, QVector<bool> discardedPeaks);
};

#endif // OESTOOLS_HPP
