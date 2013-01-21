/*
 *  Manjaro Installation Framework
 *  Roland Singer <roland@manjaro.org>
 *
 *  Copyright (C) 2007 Free Software Foundation, Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"


//###
//### Private variables
//###

QHash<QString, QHash<QString, QList<Global::Locale> > > Global::locales;
QHash<QString, QList<Global::Location> > Global::locations;



//###
//### Public methods
//###


void Global::init() {
    // TODO: Error handling
    initLocales();
    initLocations();
}



QHash<QString, QHash<QString, QList<Global::Locale> > > Global::getLocales() {
    return locales;
}



QHash<QString, QList<Global::Location> > Global::getLocations() {
    return locations;
}



//###
//### Private methods
//###


void Global::initLocales() {
    locales.clear();

    QStringList files = QDir(LOCALESDIR).entryList(QDir::Files, QDir::Name);

    for (int i = 0; i < files.size(); ++i) {
        QString filename = files.at(i);
        QFile file(QString(LOCALESDIR) + "/" + filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream in(&file);
        QString commentChar = "%";
        Locale locale;
        QString lang, territory;

        locale.locale = filename;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            QStringList split = line.split(commentChar, QString::KeepEmptyParts).first().split(QRegExp(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)"), QString::SkipEmptyParts);

            if (split.size() < 2)
                continue;

            QString sub1 = QString(split.at(0)).remove("\"");
            QString sub2 = QString(split.at(1)).remove("\"");

            if (sub1 == "comment_char")
                commentChar = sub2;
            else if (sub1 == "title")
                locale.description = sub2;
            else if (sub1 == "territory")
                territory= sub2;
            else if (sub1 == "language")
                lang = sub2;
        }

        if (lang.isEmpty() || territory.isEmpty())
            continue;

        locales[lang][territory].append(locale);
    }
}



void Global::initLocations() {
    locations.clear();

    QFile file(TZ_DATA_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed().split('#', QString::KeepEmptyParts).first().trimmed();
        if (line.isEmpty())
            continue;

        QStringList list = line.split(QRegExp("[\t ]"), QString::SkipEmptyParts);
        if (list.size() < 3)
            continue;

        Location location;
        QStringList timezone = list.at(2).split('/', QString::SkipEmptyParts);
        int cooSplitPos = QString(list.at(1)).remove(0, 1).indexOf(QRegExp("[-+]")) + 1;

        if (timezone.size() < 2)
            continue;

        location.region = timezone.first();
        location.zone = timezone.last();
        location.latitude = getRightGeoLocation(list.at(1).mid(0, cooSplitPos));
        location.longitude = getRightGeoLocation(list.at(1).mid(cooSplitPos));

        locations[location.region].append(location);
    }
}



double Global::getRightGeoLocation(QString str) {
    double sign = 1, num = 0.00;

    // Determind sign
    if (str.startsWith('-')) {
        sign = -1;
        str.remove(0, 1);
    }
    else if (str.startsWith('+')) {
        str.remove(0, 1);
    }


    if (str.length() == 4 || str.length() == 6)
        num = str.mid(0, 2).toDouble() + str.mid(2, 2).toDouble() / 60;
    else if (str.length() == 5 || str.length() == 7)
        num = str.mid(0, 3).toDouble() + str.mid(3, 2).toDouble() / 60;

    return sign * num;
}
