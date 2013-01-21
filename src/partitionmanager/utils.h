/*
 *  Manjaro Parted Backend
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

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QRegExp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "const.h"

namespace MParted {


class Utils
{
public:
    static QString charToString(const char *c);
    static QString charToStringFree(char *c);
    static Sector round(double double_value);

    static QString getFormattedSize(MParted::Sector sectors, MParted::Byte_Value sector_size);
    static double sectorToUnit(Sector sectors, Byte_Value sector_size, SIZE_UNIT size_unit);
    static QString getFilesystemString(MParted::FILESYSTEM filesystem);
    static QString getFilesystemColorCode(MParted::FILESYSTEM filesystem);

    static int executeCommand(const QString & command, const QStringList & arguments);
    static int executeCommand(const QString & command, const QStringList & arguments, QString & output, QString & error);
    static QString findProgramInPath(QString program);

    static QString getRegExp(const QString &str, QString pattern, bool splitNewLines = true);
};


}

#endif // UTILS_H
