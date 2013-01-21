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

#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include "../const.h"
#include "../utils.h"
#include "../device.h"
#include "../partition.h"
#include "../unallocatedutils.h"


namespace MParted {


class Operation
{
public:
    Operation();
    virtual ~Operation() {}

    virtual bool applyToVisual(Partitions & partitions) = 0;
    virtual QString createDescription() = 0;

    Device device;
    OperationType type;
    Partition partition_original;
    Partition partition_new;

protected:
    int findIndexOriginal(Partitions & partitions);
    int findIndexExtended(Partitions & partitions);

};


}

#endif // OPERATION_H
