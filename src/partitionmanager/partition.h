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

#ifndef PARTITION_H
#define PARTITION_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QColor>
#include "const.h"
#include "utils.h"


namespace MParted {


class Partition
{
public:
    Partition();
    bool operator==( const Partition & partition ) const;
    bool operator!=( const Partition & partition ) const;

    QString getFilesystemString() { return Utils::getFilesystemString(filesystem); }
    QColor getFilesystemColor() { return QColor(Utils::getFilesystemColorCode(filesystem)); }
    QString getFormattedSize() { return Utils::getFormattedSize(getSectorLength(), sector_size); }
    bool hasLogicalPartitions();

    void setPartitionAsUnallocated(const QString & device_path, Sector sector_start, Sector sector_end, Byte_Value sector_size, bool insideExtended);
    void updateNumber(int newNumber);
    void setSectorUsage(Sector sectors_fs_size, Sector sectors_fs_unused);

    MParted::Sector getSectorLength() const;
    bool sectorUsageKnown() const;
    MParted::Sector estimatedMinSize() const;
    MParted::Sector getSectorsUsed() const;
    MParted::Sector getSectorsUnused() const;
    MParted::Sector getSectorsUnallocated() const;
    MParted::Sector getSector() const;
    MParted::Byte_Value getByteLength() const;

    // Public Variables
    int partitionNumber;
    PartitionStatus status;
    PartitionType type;// UNALLOCATED, PRIMARY, LOGICAL, etc...
    PartitionAlignment alignment;   //ALIGN_CYLINDER, ALIGN_STRICT, etc
    FILESYSTEM filesystem;
    QString devicePath, path, label, uuid;
    Sector sector_start, sector_end, sectors_used, sectors_unused;
    Sector sectors_unallocated;  //Difference between the size of the partition and the file system
    Sector significant_threshold;  //Threshold from intrinsic to significant unallocated sectors
    bool busy, insideExtended;

    Partitions logicals;

    Sector freeSpaceBefore;  //Free space preceding partition value
    Byte_Value sector_size;  //Sector size of the disk device needed for converting to/from sectors and bytes.

private:
    void reset();
    MParted::Sector calcSignificantUnallocatedSectors() const;
};


}

#endif // PARTITION_H
