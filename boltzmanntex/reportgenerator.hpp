/*
 * Report Generation Helper Functions
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

#ifndef REPORTGENERATOR_HPP
#define REPORTGENERATOR_HPP

#include <QPrinter>
#include <QTextDocument>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QTableWidget>
#include "dataset.hpp"
#include "version.hpp"

class ReportGenerator
{
public:
    QString const headerImageLocation = ":/images/report_logo.png";
    QString const customDataUrlPrefix = "customdata://";
    QString docData;
    QTextStream doc;
    QTextDocument &html;
    QPrinter &printer;

    void insertTimestamp()
    {
        doc << QDateTime::currentDateTime().toString("ddd, dd MMMM yyyy hh:mm:ss");
    }

    void insertHeading(int level, QString str)
    {
        if (level < 1 || level > 6) {
            qDebug() << "Bug: Invalid heading level";
            return;
        }
        doc << "<h" << level << ">" << str << "</h" << level << ">";
    }

    void insertHorizontalRule() {
        doc << "<hr>";
    }

    QString insertPixmap(QPixmap &image, QString name)
    {
        QString url = customDataUrlPrefix + name;

        html.addResource(QTextDocument::ImageResource,
                         QUrl(url),
                         QVariant(image));

        doc << "<img src=\""
            << url
            << "\" width=\""
            << image.width()
            << "\" height=\""
            << image.height()
            << "\" />";

        return url;
    }

    void insertTableFromWidget(QTableWidget &t, bool checkableCells = false)
    {
        doc << "<br /><table border=\"1\" cellspacing=\"0\" >"
            << "<tr>";

        for (int i = 0; i < t.columnCount(); i++) {
            doc << "<th>" << t.horizontalHeaderItem(i)->data(Qt::DisplayRole).toString() << "</th>";
        }

        doc << "</tr>";

        for (int i = 0; i < t.rowCount(); i++) {
            doc << "<tr>";
            for (int j = 0; j < t.columnCount(); j++) {
                if ((t.item(i, j)->flags() & Qt::ItemIsUserCheckable) && checkableCells) {
                    doc  << "<td>" << (t.item(i, j)->checkState() == Qt::Checked ? "Yes" : "No")<< "</td>";
                } else {
                    doc << "<td>" << t.item(i, j)->data(Qt::DisplayRole).toString() << "</td>";
                }
            }
            doc << "</tr>";
        }

        doc << "</table><br /><br /><br />";
    }

    void insertTableFromDataset(Dataset &d)
    {
        doc << "<br /><table border=\"1\" cellspacing=\"0\" >"
            << "<tr>";

        for (int i = 0; i < d.numColumns(); i++) {
            doc << "<th>" << d[i].name << "</th>";
        }

        doc << "</tr>";

        for (int i = 0; i < d.numRows(); i++) {
            doc << "<tr>";
            for (int j = 0; j < d.numColumns(); j++) {
                doc << "<td>" << d[j][i] << "</td>";
            }
            doc << "</tr>";
        }

        doc << "</table><br /><br /><br />";
    }

    void insertPageHeader(QString title)
    {
        doc << "<p style='color: grey;'><i>";
        doc << "BoltzmannTex Version " << BOLTZMANNTEX_VERSON
            << " (" << BOLTZMANNTEX_BUILD_NUMBER
            << ", " << BOLTZMANNTEX_BUILD_COMMIT_DATE
            << ")";
        doc << "</i></p>";

        insertHeading(2, title);
        insertHorizontalRule();
    }

    void insertReportHeader(QString title)
    {

        // Header Image
        QImage headerImage(headerImageLocation);

        float height = printer.pageRect(QPrinter::DevicePixel).width() *
                      ((float) headerImage.height() /
                       (float) headerImage.width());

        doc << "<img width=\""
            << printer.pageRect(QPrinter::DevicePixel).width()
            << "\" height=\""
            << height
            << "\"  src=\""
            << headerImageLocation
            << "\" />";

        doc << "<p style='text-align: right; color: grey;'><i>";
        doc << " Version " << BOLTZMANNTEX_VERSON
            << " (" << BOLTZMANNTEX_BUILD_NUMBER
            << ", " << BOLTZMANNTEX_BUILD_COMMIT_DATE
            << ")";
        doc << "</i></p>";

        insertHeading(1, title);
        doc << "Report Generated ";
        insertTimestamp();
        insertHorizontalRule();
    }

    ReportGenerator(QTextDocument &html, QPrinter &printer):
        doc(&docData), html(html), printer(printer) {}
};

#endif // REPORTGENERATOR_HPP
