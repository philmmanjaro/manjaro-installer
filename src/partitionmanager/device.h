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

#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QList>
#include "const.h"
#include "partition.h"


namespace MParted {


class Device
{
public:
    Device();
    bool operator==(const MParted::Device & device) const;
    bool operator!=(const MParted::Device & device) const;

    QString getFormattedSize() { return Utils::getFormattedSize(length, sector_size); }

    bool hasExtendedPartition();

    // Public Variables
    Partitions partitions;
    Sector length, heads, sectors, cylinders, cylsize;
    QString path, model, diskType;
    int sector_size, maxPrimaryPartitions;
    bool unkownPartitionTable, removable;

private:
    void reset();
};


}

#endif // DEVICE_H
