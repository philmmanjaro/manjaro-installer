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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>
#include <QStringList>
#include "../const.h"
#include "../utils.h"
#include "../partition.h"

namespace MParted {


class Filesystem
{   
public:
    Filesystem();

    MParted::FSSupport getFilesystemSupport() { return fsSupport; }
    MParted::FILESYSTEM getFilesystemType() { return filesystem; }
    Byte_Value getFilesystemMAX() { return fsMAX; }
    Byte_Value getFilesystemMIN() { return fsMIN; }

    virtual void setUsedSectors(Partition & partition) = 0 ;
    virtual void readUUID(Partition & partition) = 0 ;
    virtual void readLabel(Partition & partition) = 0 ;
    virtual bool writeLabel(Partition & partition) = 0 ;
    virtual bool create(Partition & new_partition) = 0 ;
    virtual bool resize(Partition & partition_new, bool fill_partition = false) = 0 ;
    virtual bool remove(Partition & partition) = 0 ;
    virtual bool checkRepair(Partition & partition) = 0 ;

protected:
    // Filesystem informations
    FILESYSTEM filesystem;
    Byte_Value fsMIN;
    Byte_Value fsMAX;
    FSSupport fsSupport;

    //those are used in several places..
    QString output, error;
    Sector T, N, S;  //File system [T]otal num of blocks, [N]um of free (or used) blocks, block [S]ize

};


}

#endif // FILESYSTEM_H
