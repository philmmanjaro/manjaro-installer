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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QMap>
#include <QRegExp>
#include <QDebug>
#include "const.h"

class Global
{
public:
    struct Locale {
        QString description, locale;
    };

    struct Location {
        QString region, zone;
        double latitude, longitude;
    };

    struct KeyboardInfo {
        QString description;
        QMap< QString, QString > variants;
    };


    static void init();
    static QHash<QString, QHash<QString, QList<Global::Locale> > > getLocales();
    static QHash<QString, QList<Global::Location> > getLocations();
    static QMap< QString, KeyboardInfo > getKeyboardLayouts();
    static QMap<QString, QString> getKeyboardModels();

private:
    static QHash<QString, QHash<QString, QList<Global::Locale> > > locales;
    static QHash<QString, QList<Global::Location> > locations;

    static void initLocales();
    static void initLocations();
    static double getRightGeoLocation(QString str);

    static QMap<QString, QString> parseKeyboardModels(QString filepath);
    static QMap< QString, KeyboardInfo > parseKeyboardLayouts(QString filepath);
};

#endif // GLOBAL_H
