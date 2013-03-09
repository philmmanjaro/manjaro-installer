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

#include "operationresize.h"


MParted::OperationResize::OperationResize(Device & device, Partition & partition_orig, Partition & partition_new)
{
    type = MParted::OPERATION_RESIZE;
    this->device = device;
    this->partition_original = partition_orig;
    this->partition_new = partition_new;
}



bool MParted::OperationResize::applyToVisual(Partitions & partitions) {
    if (partition_original.type == MParted::TYPE_EXTENDED)
        return applyExtendedToVisual(partitions);
    else
        return applyNormalToVisual(partitions);
}



QString MParted::OperationResize::createDescription() {
    if (partition_new.getSectorLength() > partition_original.getSectorLength()) {
        //Grow partition
        return QObject::tr("Grow %1 from %2 to %3").arg(partition_original.path,
                                                        partition_original.getFormattedSize(),
                                                        partition_new.getFormattedSize());
    }
    else if (partition_new.getSectorLength() < partition_original.getSectorLength()) {
        //Shrink partition
        return QObject::tr("Shrink %1 from %2 to %3").arg(partition_original.path,
                                                        partition_original.getFormattedSize(),
                                                        partition_new.getFormattedSize());
    }
    else {
        return QObject::tr("Resize %1 (new and old partition have the same size and position. Hence continuing anyway)").arg(partition_original.path);
    }
}



bool MParted::OperationResize::applyNormalToVisual(Partitions & partitions) {
    int index = -1;

    if (partition_original.insideExtended) {
        int index_extended = findIndexExtended(partitions);

        if (index_extended >= 0)
            index = findIndexOriginal(partitions[index_extended].logicals);

        if (index < 0)
            return false;

        partitions[index_extended].logicals[index] = partition_new;
    }
    else {
        index = findIndexOriginal(partitions);

        if (index < 0)
            return false;

        partitions[index] = partition_new;
    }

    // Finally update unallocated space
    UnallocatedUtils::updateUnallocated(device, partitions);

    return true;
}



bool MParted::OperationResize::applyExtendedToVisual(Partitions & partitions) {
    int index_extended = findIndexExtended(partitions);

    if (index_extended < 0)
        return false;

    partitions[index_extended].sector_start = partition_new.sector_start;
    partitions[index_extended].sector_end = partition_new.sector_end;

    // Finally update unallocated space
    UnallocatedUtils::updateUnallocated(device, partitions);

    return true;
}
