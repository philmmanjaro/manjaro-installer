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

#ifndef MPARTED_VIRTUAL_H
#define MPARTED_VIRTUAL_H

#include <QString>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <stdio.h>
#include <math.h>
#include "mparted_core.h"
#include "const.h"
#include "utils.h"
#include "operations/operation.h"
#include "operations/operationremove.h"
#include "operations/operationcreate.h"


namespace MParted {


class MParted_Virtual
{
public:
    struct PARTITION_CREATE_RANGES {
        unsigned long MIN_FS_SIZE_MB, MAX_FS_SIZE_MB;
        unsigned long MIN_SPACE_BEFORE_MB, MAX_SPACE_BEFORE_MB;
        unsigned long MIN_SPACE_AFTER_MB, MAX_SPACE_AFTER_MB;

        PARTITION_CREATE_RANGES() {
            MIN_FS_SIZE_MB = MAX_FS_SIZE_MB = MIN_SPACE_BEFORE_MB = MAX_SPACE_BEFORE_MB = MIN_SPACE_AFTER_MB = MAX_SPACE_AFTER_MB = 0;
        }
    };

    MParted_Virtual();
    ~MParted_Virtual();

    MParted::Devices getDevices() { return devices; }

    void scanAndReset();
    bool applyToDisk();

    bool isExtendedPartitionCreatable(const MParted::Partition & unallocatedPartition);

    MParted::MParted_Virtual::PARTITION_CREATE_RANGES getCreatePartitionRanges(const MParted::Partition & unallocatedPartition, MParted::FILESYSTEM filesystem);

    bool removePartition(MParted::Partition & partition);
    bool createPartition(MParted::Partition & unallocatedPartition, unsigned long MBytesBefore,
                         unsigned long MBytesSize, MParted::FILESYSTEM filesystem,
                         bool asExtended, MParted::PartitionAlignment alignment = MParted::ALIGN_MEBIBYTE, QString label="");

private:
    QMutex mutex;
    MParted::Devices devices, devices_orig;
    QList<MParted::Operation*> operations;

    int findDeviceIndex(const MParted::Partition & partition);
    int lowestPossiblePartitionCount();
    bool addOperation(MParted::Operation *operation, MParted::Device & device);
    bool rebuildVisual();
    void mergeOperations();

    int MBNeededForBootRecord(const MParted::Partition & partition);

};


}

#endif // MPARTED_VIRTUAL_H
