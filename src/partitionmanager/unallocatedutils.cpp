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


#include "unallocatedutils.h"



void MParted::UnallocatedUtils::updateUnallocated(MParted::Device & device) {
    updateUnallocated(device, device.partitions);
}



void MParted::UnallocatedUtils::updateUnallocated(MParted::Device & device, MParted::Partitions & partitions) {
    // First remove all unallocated partitions
    removeUnallocated(partitions);

    // Insert unallocated partitions to extended section
    for (int i = 0; i < partitions.size(); i++) {
        MParted::Partition &partition = partitions[i];

        if (partition.type != MParted::TYPE_EXTENDED)
            continue;

        insertUnallocated(device, partition.logicals, partition.sector_start, partition.sector_end, device.sector_size, true);
    }

    // Insert unallocated partitions to primary partitions
    insertUnallocated(device, partitions, 0, device.length -1, device.sector_size, false);
}



void MParted::UnallocatedUtils::removeUnallocated(MParted::Partitions & partitions) {
    // Remove all unallocated partitions
    Partitions::iterator iter = partitions.begin();

    while (iter != partitions.end()) {
        if ((*iter).type == MParted::TYPE_UNALLOCATED) {
            iter = partitions.erase(iter);
        }
        else {
            if (!(*iter).logicals.isEmpty())
                removeUnallocated((*iter).logicals);

            ++iter;
        }
    }
}



void MParted::UnallocatedUtils::insertUnallocated(Device & device, Partitions & partitions, Sector start, Sector end, int sector_size, bool insideExtended) {
    MParted::Partition partition;
    partition.setPartitionAsUnallocated(device.path, 0, 0, sector_size, insideExtended);


    //if there are no partitions at all..
    if (partitions.empty()) {
        partition.sector_start = start;
        partition.sector_end = end;

        partitions.append(partition);
        return;
    }

    //start <---> first partition start
    if ((partitions.front().sector_start - start) > (MEBIBYTE / sector_size)) {
        partition.sector_start = start ;
        partition.sector_end = partitions.front().sector_start -1;

        partitions.insert(partitions.begin(), partition);
    }

    //look for gaps in between
    for (int t = 0; t < partitions.size() -1; t++)
    {
        if (   ( ( partitions[ t + 1 ] .sector_start - partitions[ t ] .sector_end - 1 ) > (MEBIBYTE / sector_size) )
            || (   ( partitions[ t + 1 ] .type != MParted::TYPE_LOGICAL )  // Only show exactly 1 MiB if following partition is not logical.
                && ( ( partitions[ t + 1 ] .sector_start - partitions[ t ] .sector_end - 1 ) == (MEBIBYTE / sector_size) )
               )
           )
        {
            partition.sector_start = partitions[ t ].sector_end +1;
            partition.sector_end = partitions[ t +1 ].sector_start -1;

            partitions.insert(partitions.begin() + ++t, partition);
        }
    }

    //last partition end <---> end
    if ((end - partitions .back() .sector_end) >= (MEBIBYTE / sector_size))
    {
        partition.sector_start = partitions .back() .sector_end +1 ;
        partition.sector_end = end ;

        partitions.append(partition);
    }
}
