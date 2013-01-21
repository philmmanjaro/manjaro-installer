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

#include "operationremove.h"


MParted::OperationRemove::OperationRemove(Device & device, Partition & partition_orig)
{
    type = MParted::OPERATION_REMOVE;

    this ->device = device;
    this ->partition_original = partition_orig;
}



bool MParted::OperationRemove::applyToVisual(Partitions & partitions) {
    int index = -1;

    if (partition_original.insideExtended) {
        int index_extended = findIndexExtended(partitions);

        if (index_extended >= 0)
            index = findIndexOriginal(partitions[index_extended].logicals);

        if (index < 0)
            return false;

        // Remove the original partition
        partitions[index_extended].logicals.removeAt(index);

        //if deleted partition was logical we have to decrease the partitionnumbers of the logicals
        //with higher numbers by one (only if its a real partition)
        if (partition_original.status != MParted::STAT_NEW) {
            for (int t = 0; t < partitions[index_extended].logicals .size(); t++) {
                if (partitions[index_extended].logicals[t].partitionNumber > partition_original.partitionNumber) {
                    partitions[index_extended].logicals[t].updateNumber(
                        partitions[index_extended].logicals[t].partitionNumber -1);
                }
            }
        }
    }
    else {
        index = findIndexOriginal(partitions);
        if (index < 0)
            return false;

        // Remove the original partition
        partitions.removeAt(index);
    }


    // Finally update unallocated space
    UnallocatedUtils::updateUnallocated(device, partitions);

    return true;
}



QString MParted::OperationRemove::createDescription() {
    QString arg1;

    if (partition_original.type == MParted::TYPE_LOGICAL)
        arg1 = QObject::tr("Logical Partition") ;
    else
        arg1 = partition_original.path;

    /*TO TRANSLATORS: looks like   Delete /dev/hda2 (ntfs, 345 MiB) from /dev/hda */
    return QObject::tr("Delete %1 (%2, %3) from %4").arg(
                    arg1,
                    partition_original.getFilesystemString(),
                    partition_original.getFormattedSize(),
                    partition_original.devicePath);
}
