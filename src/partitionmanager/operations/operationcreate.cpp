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

#include "operationcreate.h"


MParted::OperationCreate::OperationCreate(Device & device, Partition & partition_orig, Partition & partition_new)
{
    type = MParted::OPERATION_CREATE;

    this ->device = device;
    this ->partition_original = partition_orig;
    this->partition_new = partition_new;
}



bool MParted::OperationCreate::applyToVisual(Partitions & partitions) {
    int index = -1;

    if (partition_original.insideExtended) {
        int index_extended = findIndexExtended(partitions);

        if (index_extended >= 0)
            index = findIndexOriginal(partitions[index_extended].logicals);

        if (index < 0)
            return false;

        // Add new partition
        partitions[index_extended].logicals[index] = partition_new;
    }
    else
    {
        index = findIndexOriginal(partitions);
        if (index < 0)
            return false;

        // Add new partition
        partitions[index] = partition_new;
    }


    // Finally update unallocated space
    UnallocatedUtils::updateUnallocated(device, partitions);

    return true;
}



QString MParted::OperationCreate::createDescription() {
    QString type;

    switch(partition_new.type) {
        case MParted::TYPE_PRIMARY	:
            type = QObject::tr("Primary Partition");
            break;
        case MParted::TYPE_LOGICAL	:
            type = QObject::tr("Logical Partition") ;
            break;
        case MParted::TYPE_EXTENDED	:
            type = QObject::tr("Extended Partition");
            break;

        default	:
            break;
    }

    /*TO TRANSLATORS: looks like   Create Logical Partition #1 (ntfs, 345 MiB) on /dev/hda */
    return QObject::tr("Create %1 #%2 (%3, %4) on %5").arg(
                    type,
                    QString::number(partition_new.partitionNumber),
                    partition_new.getFilesystemString(),
                    partition_new.getFormattedSize(),
                    device.path);
}
