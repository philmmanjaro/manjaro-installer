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

#ifndef MPARTED_H
#define MPARTED_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QByteArray>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include <parted.h>
#include <ctime>
#include <unistd.h>
#include "const.h"
#include "utils.h"
#include "unallocatedutils.h"
#include "device.h"
#include "partition.h"

#include "operations/operation.h"

#include "filesystems/filesystem.h"
#include "filesystems/ext4.h"


namespace MParted {


class MParted_Core
{
    friend class MParted_Virtual;
public:
    static void init();
    static void release();
    static void scan(MParted::Devices & devices);
    static bool applyOperationToDisk(MParted::Operation * operation);

    static QStringList getPartitionTableTypes();
    static QList<FILESYSTEM> getSupportedFormatFilesystems();

private:
    static QMutex mutex;
    static PedDevice *pedDevice;
    static PedDisk *pedDisk;
    static QList<Filesystem*> filesystems;

    // PedDisk & PedDevice methods
    static bool openDevice(const QString & devicePath);
    static bool openDeviceAndDisk(const QString & devicePath, bool strict = true);
    static void closeDisk();
    static void closeDeviceAndDisk();
    static bool commit();
    static void settleDevice(std::time_t timeout);
    static bool isDeviceRemovable(MParted::Device & device);

    // Partition methods
    static void scanDevicePartitions(MParted::Device &device);
    static MParted::FILESYSTEM getFilesystem(PedPartition *pedPartition);
    static bool snapToAlignment(const MParted::Device & device, MParted::Partition & partition);
    static bool snapToCylinder(const MParted::Device & device, MParted::Partition & partition);
    static bool snapToMebibyte(const MParted::Device & device, MParted::Partition & partition);

    // Operation methods
    static bool calibratePartition(MParted::Partition & partition);
    static bool removePartition(MParted::Partition & partition);
    static bool removeFilesystem(MParted::Partition & partition);
    static bool create(const MParted::Device & device, MParted::Partition & new_partition);
    static bool createPartition(MParted::Partition & new_partition, MParted::Sector min_size = 0);
    static bool createFilesystem(MParted::Partition & partition);
    static bool setPartitionType(MParted::Partition & partition);
    static bool resize(const MParted::Device & device, MParted::Partition & partition_old, MParted::Partition & partition_new);
    static bool calculateExactGeom(MParted::Partition & partition_old, MParted::Partition & partition_new);
    static bool resizePartition(MParted::Partition & partition_old, MParted::Partition & partition_new);
    static bool maximizeFilesystem(MParted::Partition & partition);
    static bool resizeFilesystem(MParted::Partition & partition_old, MParted::Partition & partition_new, bool fill_partition = false);
    static bool checkRepairFilesystem(MParted::Partition & partition);

};


}

#endif // MPARTED_H
