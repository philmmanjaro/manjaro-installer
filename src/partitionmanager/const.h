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

#ifndef MPARTED_CONST_H
#define MPARTED_CONST_H

#include <QString>
#include <QList>
#include <QStringList>


namespace MParted {



typedef long long Sector;
typedef long long Byte_Value;

class Device;
typedef QList<Device> Devices;

class Partition;
typedef QList<Partition> Partitions;

//Size units defined in bytes
const Byte_Value KIBIBYTE=1024;
const Byte_Value MEBIBYTE=(KIBIBYTE * KIBIBYTE);
const Byte_Value GIBIBYTE=(MEBIBYTE * KIBIBYTE);
const Byte_Value TEBIBYTE=(GIBIBYTE * KIBIBYTE);


enum OperationType {
    OPERATION_REMOVE	= 0,
    OPERATION_CREATE	= 1,
    OPERATION_RESIZE	= 2,
    OPERATION_FORMAT	= 3
};

enum PartitionType {
    TYPE_PRIMARY		=	0,
    TYPE_LOGICAL		=	1,
    TYPE_EXTENDED		=	2,
    TYPE_UNALLOCATED	=	3
};

enum PartitionStatus {
    STAT_REAL	=	0,
    STAT_NEW	=	1,
    STAT_FORMATTED	=	2
};

enum PartitionAlignment {
    ALIGN_CYLINDER = 0,    //Align to nearest cylinder
    ALIGN_MEBIBYTE = 1    //Align to nearest mebibyte
};

enum SIZE_UNIT
{
    UNIT_SECTOR	= 0,
    UNIT_BYTE	= 1,

    UNIT_KIB	= 2,
    UNIT_MIB	= 3,
    UNIT_GIB	= 4,
    UNIT_TIB	= 5
};

enum FILESYSTEM {
    FS_UNALLOCATED	= 0,
    FS_UNKNOWN	= 1,
    FS_UNFORMATTED	= 2,
    FS_EXTENDED	= 3,

    FS_BTRFS	= 4,
    FS_EXFAT	= 5, /* Also known as fat64 */
    FS_EXT2		= 6,
    FS_EXT3		= 7,
    FS_EXT4		= 8,
    FS_FAT16	= 9,
    FS_FAT32	= 10,
    FS_HFS		= 11,
    FS_HFSPLUS	= 12,
    FS_JFS		= 13,
    FS_LINUX_SWAP	= 14,
    FS_LVM2_PV	= 15,
    FS_NILFS2	= 16,
    FS_NTFS		= 17,
    FS_REISER4	= 18,
    FS_REISERFS	= 19,
    FS_UFS		= 20,
    FS_XFS		= 21,

    FS_USED		= 22,
    FS_UNUSED	= 23,

    FS_LUKS		= 24
};



// Struct to store file system information
struct FSSupport {
    enum Support {
        NONE		= 0,
        EXTERNAL	= 1
    };

    Support read_used_sectors ;
    Support read_uuid ;
    Support read_label ;
    Support write_label ;
    Support create ;
    Support grow ;
    Support shrink ;
    Support check ;

    FSSupport() {
        read_used_sectors = read_label = write_label = read_uuid = create = grow = shrink = check = NONE ;
    }
};



}

#endif // MPARTED_CONST_H
