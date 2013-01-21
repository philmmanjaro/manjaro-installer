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

#include "operation.h"


MParted::Operation::Operation()
{
}



int MParted::Operation::findIndexOriginal(Partitions & partitions) {
    for ( int t = 0 ; t < partitions.size() ; t++ )
        if ( partition_original.sector_start >= partitions[t].sector_start &&
             partition_original.sector_end <= partitions[t].sector_end )
            return t;

    return -1;
}



int MParted::Operation::findIndexExtended(Partitions & partitions) {
    for ( int t = 0 ; t < partitions.size() ; t++ )
        if ( partitions[t].type == MParted::TYPE_EXTENDED )
            return t;

    return -1;
}
