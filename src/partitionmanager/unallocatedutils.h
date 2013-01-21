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

#ifndef UNALLOCATEDUTILS_H
#define UNALLOCATEDUTILS_H

#include <QString>
#include "const.h"
#include "device.h"
#include "partition.h"


namespace MParted {


class UnallocatedUtils
{
public:
    static void updateUnallocated(MParted::Device & device);
    static void updateUnallocated(MParted::Device & device, MParted::Partitions & partitions);
    static void removeUnallocated(MParted::Partitions & partitions);
    static void insertUnallocated(Device & device, Partitions & partitions, Sector start, Sector end, int sector_size, bool insideExtended);

};


}

#endif // UNALLOCATEDUTILS_H
