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

#include "operationformat.h"


MParted::OperationFormat::OperationFormat(Device & device, Partition & partition_orig, Partition & partition_new)
{
    type = MParted::OPERATION_FORMAT;

    this->device = device;
    this->partition_original = partition_orig;
    this->partition_new = partition_new;
}



bool MParted::OperationFormat::applyToVisual(Partitions & partitions) {
    int index = -1;

    if (partition_original.insideExtended) {
        int index_extended = findIndexExtended(partitions);

        if (index_extended >= 0)
            index = findIndexOriginal(partitions[index_extended].logicals);

        if (index < 0)
            return false;

        partitions[index_extended].logicals[index] = partition_new;
    }
    else
    {
        index = findIndexOriginal(partitions);
        if (index < 0)
            return false;

        partitions[index] = partition_new;
    }

    return true;
}



QString MParted::OperationFormat::createDescription() {
    /*TO TRANSLATORS: looks like  Format /dev/hda4 as linux-swap */
    return QObject::tr("Format %1 as %2").arg(
                    partition_original.path,
                    partition_new.getFilesystemString());
}
