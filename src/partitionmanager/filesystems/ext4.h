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

#ifndef EXT4_H
#define EXT4_H

#include <QString>
#include "filesystem.h"


namespace MParted {


class ext4 : public Filesystem
{
public:
    ext4();

    void setUsedSectors(Partition & partition);
    void readUUID(Partition & partition);
    void readLabel(Partition & partition);
    bool writeLabel(Partition & partition);
    bool create(Partition & new_partition);
    bool resize(Partition & partition_new, bool fill_partition = false);
    bool remove(Partition & partition);
    bool checkRepair(Partition & partition);

};


}

#endif // EXT4_H
