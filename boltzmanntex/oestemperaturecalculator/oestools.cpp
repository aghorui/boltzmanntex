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

#include "oestools.hpp"

int OESTools::integrate(
        List const &wavelength,
        List const &counts,
        List const &peakStart,
        List const &peakEnd,
        IntegrationResult &result)
{
    result.peakAreas.clear();
    result.noise.clear();

    std::cout << std::setw (10) << "Peak N.\t"
              << std::setw (10) << "Low\t"
              << std::setw (10) << "High\t"
              << std::setw (10) << "Area\t"
              << std::setw (10) << "Noise\t"
              << std::setw (10) << "Final area" << std::endl;

    for (int peakIndex = 0; peakIndex < peakStart.size(); peakIndex++) {
        double area = 0;
        DataRange range = find_range(wavelength, peakStart[peakIndex], peakEnd[peakIndex]);

        if (range.start < 0 || range.end < 0) {
            return -1;
        }

        for (Index i = range.start; i <= (range.end - 1); i++) {
            // Trapezoidal integration. 1/2 * (base1 + base2) * (height)
            // 	IF(IP.EQ.8)BKG=(WP(IJ)-WP(1))*AP(IJ)
            //  IF(IP.EQ.7)BKG=(WP(IJ)-WP(1))*BGH7
            //qDebug() << "W" << wavelength[i + 1] <<  wavelength[i] << counts[i] << counts[i + 1];
            area += (wavelength[i + 1] - wavelength[i]) *
                    (counts[i] + counts[i + 1]) / 2.0;
        }

        //double bemstrahlung = (peak_end_wavelength - peak_start_wavelength) * peak_start_count;
        double bemstrahlung = (wavelength[range.end] - wavelength[range.start]) * counts[range.start];
        //double final_area = area - bemstrahlung;
        double final_area = area - bemstrahlung;

        std::cout << std::setw (10) << peakIndex << "\t"
                  << std::setw (10) << peakStart[peakIndex] << "\t"
                  << std::setw (10) << peakEnd[peakIndex] << "\t"
                  << std::setw (10) << area << "\t"
                  << std::setw (10) << bemstrahlung << "\t"
                  << std::setw (10) << final_area << "\t" << std::endl;

        result.peakAreas.push_back(final_area);
        result.noise.push_back(bemstrahlung);
    }
    return 0;
}

int OESTools::regression(
        List const &peakAreas,
        List const &spectralPeakWavelength,
        List const &transitionProbability,
        List const &upperLevelEnergy,
        List const &degeneracy,
        QVector<bool> const &discardedPeaks,
        RegressionResult &result)
{
    result.regressionPointX.clear();
    result.regressionPointY.clear();
    result.regressionLineY.clear();

    for (int i = 0; i < peakAreas.size(); i++) {
        /* Wl (nm)  Ak (s-1)   Ek (cm^-1)  Gk Degen (2*J + 1) */
        double arg = (transitionProbability[i] * degeneracy[i] / (peakAreas[i] * spectralPeakWavelength[i]));
        //arg=1/arg
        //Y(I)=ALOG(ARG)
        //X(I)=EK(I)*hc*1e2/1.6e-19
        result.regressionPointY.push_back(log(1/arg));
        result.regressionPointX.push_back(upperLevelEnergy[i] * HC * 1e2 / 1.6e-19);
        qDebug() << upperLevelEnergy[i] * HC * 1e2 / 1.6e-19 << log(1/arg);
      }

//    qDebug() << "Discarded:";
//    for (int i = 0; i < discardedPeaks.size(); i++) {
//        if (discardedPeaks[i] == false)
//            continue;
//        qDebug() << i;
//        qDebug() << "DISCARD" << result.regressionPointX[i] << result.regressionPointY[i];
//        result.discardPointX.push_back(result.regressionPointX[i]);
//        result.regressionPointX.remove(i);
//        result.discardPointY.push_back(result.regressionPointY[i]);
//        result.regressionPointY.remove(i);
//    }

    double a1 = 0, a2 = 0, a3 = 0,
           c1 = 0, c2 = 0, c3 = 0;

    for (int i = 0;  i < result.regressionPointX.size(); i++) {
        qDebug() << result.regressionPointY[i] << result.regressionPointX[i];
    }

    for (int i = 0; i < result.regressionPointX.size(); i++) {
        if (discardedPeaks[i] == true) {
            continue;
        }
        a1 += 1;
        a2 = a2 + result.regressionPointX[i];
        a3 = a3 + result.regressionPointY[i];
        c1 = c1 + result.regressionPointX[i];
        c2 = c2 + result.regressionPointX[i] * result.regressionPointX[i];
        c3 = c3 + result.regressionPointX[i] * result.regressionPointY[i];
    }

    a3 = -a3;
    c3 = -c3;

    double b1 = (a3 * c1 - a1 * c3) / (a1 * c2 - a2 * c1);
    double b0 = -(a3 + a2 * b1) / a1;

    for (int i = 0; i < result.regressionPointX.size(); i++) {
        result.regressionLineY.push_back(b0 + b1 * result.regressionPointX[i]);
    }

    qDebug() << "B1 = " << b1;
    result.temperature = -1.6e-19 / (K * b1);

    qDebug() << "\nTEMPERATURE:" << result.temperature;
    return 0;
}

static double meanDiffSquareSum(QVector<double> list, QVector<bool> discardedPeaks)
{
    double n = 0;
    double avg = 0;
    double sum = 0.0;

    for (int i = 0; i < list.size(); i++) {
        if (discardedPeaks[i]) {
            continue;
        }
        n += 1;
        avg += list[i];
    }

    avg /= n;

    for (int i = 0; i < list.size(); i++) {
        if (discardedPeaks[i])
            continue;
        sum += (list[i] - avg) * (list[i] - avg);
    }

    return sum;
}

static double meanDiffSum2(QVector<double> list1, QVector<double> list2, QVector<bool> discardedPeaks)
{
    double n = 0;
    double avg1 = 0;
    double avg2 = 0;

    for (int i = 0; i < list1.size(); i++) {
        if (discardedPeaks[i]) {
            continue;
        }
        n += 1;
        avg1 += list1[i];
        avg2 += list2[i];
    }

    avg1 /= n;
    avg2 /= n;

    qDebug() << avg1 << avg2;
    double sum = 0.0;

    for (int i = 0; i < list1.size(); i++) {
        if (discardedPeaks[i])
            continue;
        sum += (list1[i] - avg1) * (list2[i] - avg2);
    }

    return sum;
}

// Calculates correlation coeff using Peterson's method
float OESTools::correlation(RegressionResult const &result, QVector<bool> discardedPeaks)
{
    double xy = meanDiffSum2(result.regressionPointX, result.regressionPointY, discardedPeaks);
    double x = meanDiffSquareSum(result.regressionPointX, discardedPeaks);
    double y = meanDiffSquareSum(result.regressionPointY, discardedPeaks);
    double n = (result.regressionPointX.size() - 1.0);
    double covariance = xy;
    double denominator = std::sqrt(x * y);

    qDebug() << xy << x << y << n << covariance << denominator;

    return covariance / denominator;
}
