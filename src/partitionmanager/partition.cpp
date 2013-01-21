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

#include "partition.h"

MParted::Partition::Partition()
{
    // Setup data
    reset();
}



bool MParted::Partition::operator==( const Partition & partition ) const {
    return devicePath == partition .devicePath &&
           partitionNumber == partition .partitionNumber &&
           sector_start == partition .sector_start &&
           type == partition .type ;
}

bool MParted::Partition::operator!=( const Partition & partition ) const {
    return ! ( *this == partition ) ;
}



bool MParted::Partition::hasLogicalPartitions() {
    for (int i = 0; i < logicals.size(); i++) {
        if (logicals.at(i).filesystem == MParted::FS_UNALLOCATED)
            continue;

        return true;
    }

    return false;
}



void MParted::Partition::setPartitionAsUnallocated(const QString & device_path, Sector sector_start, Sector sector_end, Byte_Value sector_size, bool insideExtended) {
    // Reset first
    this->reset();

    // Set data
    this->devicePath = device_path;
    this->path = Utils::getFilesystemString(MParted::FS_UNALLOCATED);
    this->partitionNumber = -1;
    this->type = MParted::TYPE_UNALLOCATED;
    this->filesystem = MParted::FS_UNALLOCATED;
    this->sector_start = sector_start;
    this->sector_end = sector_end;
    this->sector_size = sector_size;
    this->insideExtended = insideExtended;
}



void MParted::Partition::updateNumber(int newNumber) {
    path = path.replace(QString::number(partitionNumber), QString::number(newNumber));
    partitionNumber = newNumber;
}



//Set file system size and free space, which also calculates unallocated
//  space.  Set sectors_fs_size = -1 for unknown.
void MParted::Partition::setSectorUsage(Sector sectors_fs_size, Sector sectors_fs_unused) {
    Sector length = getSectorLength() ;
    if (    0 <= sectors_fs_size   && sectors_fs_size   <= length
         && 0 <= sectors_fs_unused && sectors_fs_unused <= sectors_fs_size
       )
    {
        sectors_used          = sectors_fs_size - sectors_fs_unused ;
        sectors_unused        = sectors_fs_unused ;
        sectors_unallocated   = length - sectors_fs_size ;
        significant_threshold = calcSignificantUnallocatedSectors() ;
    }
    else if ( sectors_fs_size == -1 )
    {
        if ( 0 <= sectors_fs_unused && sectors_fs_unused <= length )
        {
            sectors_used   = length - sectors_fs_unused ;
            sectors_unused = sectors_fs_unused ;
        }
        else
        {
             sectors_used = -1 ;
             sectors_unused = -1 ;
        }
        sectors_unallocated   = 0 ;
        significant_threshold = 1 ;
    }
}



MParted::Sector MParted::Partition::getSectorLength() const {
    if ( sector_start >= 0 && sector_end >= 0 )
        return sector_end - sector_start + 1 ;
    else
        return -1 ;
}



bool MParted::Partition::sectorUsageKnown() const {
    return sectors_used >= 0 && sectors_unused >= 0 ;
}



MParted::Sector MParted::Partition::estimatedMinSize() const {
    //Add unallocated sectors up to the significant threshold, to
    //  account for any intrinsic unallocated sectors in the
    //  file systems minimum partition size.
    if ( sectors_used >= 0 )
        return sectors_used + std::min( sectors_unallocated, significant_threshold ) ;
    return -1 ;
}



//Return user displayable used sectors.
//  Only add unallocated sectors up to the significant threshold to
//  account for any intrinsic unallocated sectors in the file system.
//  Above the threshold just return the used sectors figure.
MParted::Sector MParted::Partition::getSectorsUsed() const {
    if ( sectors_used >= 0 )
    {
        if ( sectors_unallocated < significant_threshold )
            return sectors_used + sectors_unallocated ;
        else
            return sectors_used ;
    }
    return -1 ;
}



//Return user displayable unused sectors.
MParted::Sector MParted::Partition::getSectorsUnused() const {
    return sectors_unused ;
}

//Return user displayable unallocated sectors.
//  Return zero below the significant unallocated sectors threshold, as
//  the figure has been added to the displayable used sectors.  Above the
//  threshold just return the unallocated sectors figure.
MParted::Sector MParted::Partition::getSectorsUnallocated() const {
    if ( sectors_unallocated < significant_threshold )
        return 0 ;
    else
        return sectors_unallocated ;
}



MParted::Sector MParted::Partition::getSector() const {
    return (sector_start + sector_end) / 2 ;
}



MParted::Byte_Value MParted::Partition::getByteLength() const {
    if (getSectorLength() >= 0)
        return getSectorLength() * sector_size;
    else
        return -1 ;
}



//###
//### Private
//###



void MParted::Partition::reset() {
    logicals.clear();

    path = devicePath = label = uuid = "";
    status = MParted::STAT_REAL;
    type = MParted::TYPE_UNALLOCATED;
    alignment = MParted::ALIGN_MEBIBYTE;
    filesystem = MParted::FS_UNALLOCATED;

    partitionNumber = sector_start = sector_end = sectors_used = sectors_unused = sector_size = -1;
    sectors_unallocated = 0;
    significant_threshold = 1;
    freeSpaceBefore = -1;
    busy = insideExtended = false;
}



//Return threshold of sectors which is considered above the intrinsic
//  level for a file system which "fills" the partition.  Calculation
//  is:
//      %age of partition size           , when
//      5%                               ,           ptn size <= 100 MiB
//      linear scaling from 5% down to 2%, 100 MiB < ptn size <= 1 GiB
//      2%                               , 1 GiB   < ptn size
MParted::Sector MParted::Partition::calcSignificantUnallocatedSectors() const {
    const double HIGHER_UNALLOCATED_FRACTION = 0.05 ;
    const double LOWER_UNALLOCATED_FRACTION  = 0.02 ;
    Sector     length   = getSectorLength() ;
    Byte_Value byte_len = length * sector_size ;
    Sector     significant ;

    if ( byte_len <= 0 )
    {
        significant = 1;
    }
    else if ( byte_len <= 100 * MEBIBYTE )
    {
        significant = Utils::round( length * HIGHER_UNALLOCATED_FRACTION ) ;
    }
    else if ( byte_len <= 1 * GIBIBYTE )
    {
        double fraction = ( HIGHER_UNALLOCATED_FRACTION - LOWER_UNALLOCATED_FRACTION ) -
                          ( byte_len - 100 * MEBIBYTE ) * ( HIGHER_UNALLOCATED_FRACTION - LOWER_UNALLOCATED_FRACTION ) /
                          ( 1 * GIBIBYTE - 100 * MEBIBYTE ) +
                          LOWER_UNALLOCATED_FRACTION ;
        significant = Utils::round( length * fraction ) ;
    }
    else
    {
        significant = Utils::round( length * LOWER_UNALLOCATED_FRACTION ) ;
    }
    if ( significant <= 1 )
        significant = 1;
    return significant ;
}
