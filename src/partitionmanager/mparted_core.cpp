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

#include "mparted_core.h"


//###
//### Static variables
//###


QMutex MParted::MParted_Core::mutex;
PedDevice *MParted::MParted_Core::pedDevice = NULL;
PedDisk *MParted::MParted_Core::pedDisk = NULL;
QList<MParted::Filesystem*> MParted::MParted_Core::filesystems;




//###
//### Publics methods
//###


void MParted::MParted_Core::init() {
    QMutexLocker locker(&mutex);

    // Init filesystems
    filesystems.append(new MParted::ext4());
}



void MParted::MParted_Core::release() {
    QMutexLocker locker(&mutex);

    qDeleteAll(filesystems);
    filesystems.clear();
}



void MParted::MParted_Core::scan(MParted::Devices & devices) {
    QMutexLocker locker(&mutex);

    // Clean up first
    devices.clear();

    // Initialise
    QStringList devicePaths;
    ped_device_probe_all();

    pedDevice = NULL;
    while ((pedDevice = ped_device_get_next(pedDevice))) {
        // Only add this device if we can read the first sector (which means it's a real device)
        char * buf = static_cast<char *>(malloc(pedDevice->sector_size));
        if (buf) {
            // Confirming device
            if (ped_device_open(pedDevice)) {
                //Devices with sector sizes of 512 bytes and higher are supported
                if (ped_device_read(pedDevice, buf, 0, 1))
                    devicePaths.append(Utils::charToString(pedDevice->path));

                ped_device_close(pedDevice) ;
            }
            free(buf);
        }
    }

    closeDeviceAndDisk();

    // Sort list, remove duplicates and empty entries
    devicePaths.removeDuplicates();
    devicePaths.removeAll("");
    devicePaths.sort();




    for (int i = 0; i < devicePaths.size(); i++) {
        const QString devicePath = devicePaths.at(i);

        // Open device and disk
        if (!openDeviceAndDisk(devicePath, false))
            continue;

        // Add to list and save pointer
        devices.append(MParted::Device());
        MParted::Device &device = devices.last();

        //device info..
        device.path = devicePath;
        device.model 	=	Utils::charToString(pedDevice->model);
        device.length 	=	pedDevice->length;
        device.sector_size	=	pedDevice->sector_size;
        device.heads 	=	pedDevice->bios_geom.heads;
        device.sectors 	=	pedDevice->bios_geom.sectors;
        device.cylinders	=	pedDevice->bios_geom.cylinders;
        device.cylsize 	=	device.heads * device.sectors;
        device.removable = isDeviceRemovable(device);


        //make sure cylsize is at least 1 MiB
        if (device.cylsize < (MEBIBYTE / device.sector_size))
            device.cylsize = (MEBIBYTE / device.sector_size);

        //normal harddisk
        if (pedDisk) {
            device.unkownPartitionTable = false;
            device.diskType =	Utils::charToString(pedDisk->type->name);
            device.maxPrimaryPartitions = ped_disk_get_max_primary_partition_count(pedDisk);

            // Scan partitions
            scanDevicePartitions(device);
        }
        //harddisk without disklabel
        else {
            device.unkownPartitionTable = true;
            device.diskType = QObject::tr("unrecognized");
            device.maxPrimaryPartitions = -1;
        }

        // Clean up
        closeDeviceAndDisk();
    }
}


// TODO add error messages!
bool MParted::MParted_Core::applyOperationToDisk(MParted::Operation * operation) {
    QMutexLocker locker(&mutex);
    bool success = false;

    if (!calibratePartition(operation->partition_original))
            return false;

    switch (operation->type)
    {
        case MParted::OPERATION_REMOVE:
            success = (removeFilesystem(operation->partition_original)
                       && removePartition(operation->partition_original));
            break;
        case MParted::OPERATION_CREATE:
            success = create(operation->device, operation->partition_new);
            break;
        case MParted::OPERATION_FORMAT:
            success = (removeFilesystem(operation->partition_original)
                       && setPartitionType(operation->partition_new)
                       && createFilesystem(operation->partition_new));
            break;
        case MParted::OPERATION_RESIZE:
            success = resize(operation->device,
                             operation->partition_original,
                             operation->partition_new);
            break;
        default:
            return false;
    }

    return success;
}



QStringList MParted::MParted_Core::getPartitionTableTypes() {
    QMutexLocker locker(&mutex);
    QStringList disklabeltypes;

    //msdos should be first in the list
    disklabeltypes.append("msdos");

    PedDiskType *disk_type ;
    for (disk_type = ped_disk_type_get_next( NULL ) ; disk_type ; disk_type = ped_disk_type_get_next( disk_type )) {
        QString type = Utils::charToString(disk_type->name);

        if (type != "msdos")
            disklabeltypes.append(type);
    }

    return disklabeltypes ;
}



QList<MParted::FILESYSTEM> MParted::MParted_Core::getSupportedFormatFilesystems() {
    QList<MParted::FILESYSTEM> list;

    // Find filesystems which are supported
    for (int i = 0; i < filesystems.size(); i++) {
        MParted::Filesystem *fs = filesystems[i];

        if (!fs->getFilesystemSupport().create
                || fs->getFilesystemType() == MParted::FS_UNKNOWN
                || fs->getFilesystemType() == MParted::FS_LUKS)
            continue;

        list.append(fs->getFilesystemType());
    }

    // Add unformatted type
    list.append(MParted::FS_UNFORMATTED);

    return list;
}



//###
//### Private - PedDisk & PedDevice methods
//###


bool MParted::MParted_Core::openDevice(const QString & devicePath) {
    QByteArray ba = devicePath.toLocal8Bit();

    pedDevice = ped_device_get(ba.constData());

    return pedDevice;
}



bool MParted::MParted_Core::openDeviceAndDisk(const QString & devicePath, bool strict) {
    if (!openDevice(devicePath))
        return false;

    pedDisk = ped_disk_new(pedDevice);

    // If ! disk and writeable it's probably a HD without disklabel.
    // We return true here
    if (pedDisk || (!strict && !pedDevice->read_only))
        return true;

    closeDeviceAndDisk();
    return false;
}



void MParted::MParted_Core::closeDisk() {
    if (pedDisk)
        ped_disk_destroy(pedDisk);

    pedDisk = NULL ;
}



void MParted::MParted_Core::closeDeviceAndDisk() {
    closeDisk() ;

    if (pedDevice)
        ped_device_destroy(pedDevice);

    pedDevice = NULL;
}



bool MParted::MParted_Core::commit() {
    if (!ped_disk_commit_to_dev(pedDisk))
        return false;

    bool success = ped_disk_commit_to_os(pedDisk);

    //Work around to try to alleviate problems caused by
    //  bug #604298 - Failure to inform kernel of partition changes
    //  If not successful the first time, try one more time.
    if (!success) {
        sleep(1);
        success = ped_disk_commit_to_os(pedDisk);
    }

    settleDevice(10);

    return success;
}



void MParted::MParted_Core::settleDevice(std::time_t timeout) {
    if (!Utils::findProgramInPath("udevsettle").isEmpty())
        Utils::executeCommand("udevsettle", QStringList() << "--timeout=" + QString::number(timeout));
    else if (!Utils::findProgramInPath("udevadm").isEmpty())
        Utils::executeCommand("udevadm", QStringList() << "settle" << "--timeout=" + QString::number(timeout));
    else
        sleep(timeout);
}



bool MParted::MParted_Core::isDeviceRemovable(MParted::Device & device) {
    QFile file(QString("/sys/block/%1/removable").arg(QString(device.path).split("/", QString::SkipEmptyParts).last()));

    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    if (QString(file.readAll()).toInt() == 1)
        return true;
    else
        return false;
}



//###
//### Private - Partition methods
//###


void MParted::MParted_Core::scanDevicePartitions(MParted::Device &device) {
    device.partitions.clear();

    // Initialise
    PedPartition *pedPartition = NULL;
    MParted::Partition *extendedPartition = NULL;

    while ((pedPartition = ped_disk_next_partition(pedDisk, pedPartition))) {
        //if there's no end, there's no partition ;)
        if (pedPartition->geom.end <= -1)
            continue;

        MParted::Partition partition;
        partition.devicePath = device.path;
        partition.sector_size = device.sector_size;
        partition.busy = ped_partition_is_busy(pedPartition);
        partition.partitionNumber = pedPartition->num;
        partition.sector_start = pedPartition->geom.start;
        partition.sector_end = pedPartition->geom.end;

        // Get partition path
        partition.path = Utils::charToStringFree(ped_partition_get_path(pedPartition)); // we have to free the result of ped_partition_get_path()
        if (partition.path.isEmpty())
            partition.path = "Partition path not found";


        switch (pedPartition->type) {
            case PED_PARTITION_LOGICAL:
                partition.insideExtended = true;

            case PED_PARTITION_NORMAL:
                partition.type = pedPartition->type == 0 ?	MParted::TYPE_PRIMARY : MParted::TYPE_LOGICAL;
                partition.filesystem = getFilesystem(pedPartition);

                // Find filesystem class and set used sectors, label and uuid
                for (int i = 0; i < filesystems.size(); i++) {
                    MParted::Filesystem *fs = filesystems[i];

                    if (fs->getFilesystemType() != partition.filesystem)
                        continue;

                    fs->setUsedSectors(partition);
                    fs->readLabel(partition);
                    fs->readUUID(partition);
                    break;
                }

                break ;
            case PED_PARTITION_EXTENDED:
                partition.type = MParted::TYPE_EXTENDED;
                partition.filesystem = MParted::FS_EXTENDED;

                break ;
            default:
                continue;
        }

        // Add partition to list
        if (partition.type == MParted::TYPE_LOGICAL && extendedPartition != NULL)
            extendedPartition->logicals.append(partition);
        else
            device.partitions.append(partition);

        // Save pointer of extended partition, if it is one
        if (partition.type == MParted::TYPE_EXTENDED)
            extendedPartition = &device.partitions.last();
    }

    // Update unallocated space
    UnallocatedUtils::updateUnallocated(device);
}



MParted::FILESYSTEM MParted::MParted_Core::getFilesystem(PedPartition *pedPartition) {
    char magic1[16] = "";
    char magic2[16] = "";

    //Check for LUKS encryption prior to libparted file system detection.
    //  Otherwise encrypted file systems such as ext3 will be detected by
    //  libparted as 'ext3'.

    //LUKS encryption
    char * buf = static_cast<char *>( malloc( pedDevice ->sector_size ) ) ;
    if ( buf ) {
        ped_device_open( pedDevice );
        ped_geometry_read( & pedPartition ->geom, buf, 0, 1 ) ;
        memcpy(magic1, buf+0, 6) ;  //set binary magic data
        ped_device_close( pedDevice );
        free( buf ) ;

        if (0 == memcmp(magic1, "LUKS\xBA\xBE", 6))
            return MParted::FS_LUKS;
    }

    QString fs_type = "" ;

    //Standard libparted file system detection
    if (pedPartition && pedPartition->fs_type)
        fs_type = pedPartition ->fs_type ->name ;

    if (!fs_type.isEmpty()) {
        if ( fs_type == "extended" )
            return MParted::FS_EXTENDED ;
        else if ( fs_type == "btrfs" )
            return MParted::FS_BTRFS ;
        else if ( fs_type == "exfat" )
            return MParted::FS_EXFAT ;
        else if ( fs_type == "ext2" )
            return MParted::FS_EXT2 ;
        else if ( fs_type == "ext3" )
            return MParted::FS_EXT3 ;
        else if ( fs_type == "ext4" ||
                  fs_type == "ext4dev" )
            return MParted::FS_EXT4 ;
        else if ( fs_type == "linux-swap" ||
                  fs_type == "linux-swap(v1)" ||
                  fs_type == "linux-swap(new)" ||
                  fs_type == "linux-swap(v0)" ||
                  fs_type == "linux-swap(old)" ||
                  fs_type == "swap" )
            return MParted::FS_LINUX_SWAP ;
        else if ( fs_type == "LVM2_member" )
            return MParted::FS_LVM2_PV ;
        else if ( fs_type == "fat16" )
            return MParted::FS_FAT16 ;
        else if ( fs_type == "fat32" )
            return MParted::FS_FAT32 ;
        else if ( fs_type == "nilfs2" )
            return MParted::FS_NILFS2 ;
        else if ( fs_type == "ntfs" )
            return MParted::FS_NTFS ;
        else if ( fs_type == "reiserfs" )
            return MParted::FS_REISERFS ;
        else if ( fs_type == "xfs" )
            return MParted::FS_XFS ;
        else if ( fs_type == "jfs" )
            return MParted::FS_JFS ;
        else if ( fs_type == "hfs" )
            return MParted::FS_HFS ;
        else if ( fs_type == "hfs+" ||
                  fs_type == "hfsplus" )
            return MParted::FS_HFSPLUS ;
        else if ( fs_type == "ufs" )
            return MParted::FS_UFS ;
    }



    //other file systems libparted couldn't detect (i've send patches for these file systems to the parted guys)
    // - no patches sent to parted for lvm2, or luks

    //reiser4
    buf = static_cast<char *>( malloc( pedDevice ->sector_size ) ) ;
    if ( buf )
    {
        ped_device_open( pedDevice );
        ped_geometry_read( & pedPartition ->geom
                         , buf
                         , (65536 / pedDevice ->sector_size)
                         , 1
                         ) ;
        memcpy(magic1, buf+0, 7) ; //set binary magic data
        ped_device_close( pedDevice );
        free( buf ) ;

        if ( 0 == memcmp( magic1, "ReIsEr4", 7 ) )
            return MParted::FS_REISER4 ;
    }

    //lvm2
    //NOTE: lvm2 is not a file system but we do wish to recognize the Physical Volume
    buf = static_cast<char *>( malloc( pedDevice ->sector_size ) ) ;
    if ( buf )
    {
        ped_device_open( pedDevice );
        if ( pedDevice ->sector_size == 512 )
        {
            ped_geometry_read( & pedPartition ->geom, buf, 1, 1 ) ;
            memcpy(magic1, buf+ 0, 8) ; // set binary magic data
            memcpy(magic2, buf+24, 4) ; // set binary magic data
        }
        else
        {
            ped_geometry_read( & pedPartition ->geom, buf, 0, 1 ) ;
            memcpy(magic1, buf+ 0+512, 8) ; // set binary magic data
            memcpy(magic2, buf+24+512, 4) ; // set binary magic data
        }
        ped_device_close( pedDevice );
        free( buf ) ;

        if (    0 == memcmp( magic1, "LABELONE", 8 )
             && 0 == memcmp( magic2, "LVM2", 4 ) )
        {
            return MParted::FS_LVM2_PV ;
        }
    }

    //btrfs
    const Sector BTRFS_SUPER_INFO_SIZE   = 4096 ;
    const Sector BTRFS_SUPER_INFO_OFFSET = (64 * 1024) ;
    const char* const BTRFS_SIGNATURE  = "_BHRfS_M" ;

    char    buf_btrfs[BTRFS_SUPER_INFO_SIZE] ;

    ped_device_open( pedDevice ) ;
    ped_geometry_read( & pedPartition ->geom
                     , buf_btrfs
                     , (BTRFS_SUPER_INFO_OFFSET / pedDevice ->sector_size)
                     , (BTRFS_SUPER_INFO_SIZE / pedDevice ->sector_size)
                     ) ;
    memcpy(magic1, buf_btrfs+64, strlen(BTRFS_SIGNATURE) ) ;  //set binary magic data
    ped_device_close( pedDevice ) ;

    if ( 0 == memcmp( magic1, BTRFS_SIGNATURE, strlen(BTRFS_SIGNATURE) ) )
    {
        return MParted::FS_BTRFS ;
    }


    //no file system found....
    return MParted::FS_UNKNOWN ;
}



bool MParted::MParted_Core::snapToAlignment(const MParted::Device & device, MParted::Partition & partition) {
    bool rc = true;

    if (partition.alignment == MParted::ALIGN_CYLINDER)
        rc = snapToCylinder(device, partition);
    else if (partition.alignment == MParted::ALIGN_MEBIBYTE)
        rc = snapToMebibyte(device, partition);

    // Ensure that partition start and end are not beyond the ends of the disk device
    if (partition.sector_start < 0)
        partition.sector_start = 0;
    if (partition.sector_end > device.length)
        partition.sector_end = device.length - 1;

    // Do some basic checks on the partition
    if (partition.getSectorLength() <= 0) {
        /* TO TRANSLATORS:  looks like   A partition cannot have a length of -1 sectors */
        // TODO: Last error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //error = QObject::tr("A partition cannot have a length of %1 sectors").arg(partition.getSectorLength());
        return false ;
    }

    //FIXME: I think that this if condition should be impossible because Partition::set_sector_usage(),
    //  and ::set_used() and ::Set_Unused() before that, don't allow setting file usage figures to be
    //  larger than the partition size.  A btrfs file system spanning muiltiple partitions will have
    //  usage figures larger than any single partition but the figures will won't be set because of
    //  the above reasoning.  Confirm condition is impossible and consider removing this code.
    if (partition.getSectorLength() < partition.sectors_used) {
        /* TO TRANSLATORS: looks like   A partition with used sectors (2048) greater than its length (1536) is not valid */
        // TODO: last error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //error = QObject::tr("A partition with used sectors (%1) greater than its length (%2) is not valid").arg(
        //    partition.sectors_used,
        //    partition.getSectorLength());
        return false;
    }

    //FIXME: it would be perfect if we could check for overlapping with adjacent partitions as well,
    //however, finding the adjacent partitions is not as easy as it seems and at this moment all the dialogs
    //already perform these checks. A perfect 'fixme-later' ;)

    return rc;
}



bool MParted::MParted_Core::snapToCylinder(const MParted::Device & device, MParted::Partition & partition) {
    MParted::Sector diff = 0;

    //Determine if partition size is less than half a disk cylinder
    bool less_than_half_cylinder = false;
    if ( ( partition .sector_end - partition .sector_start ) < ( device .cylsize / 2 ) )
        less_than_half_cylinder = true;

    if ( partition.type == MParted::TYPE_LOGICAL ||
         partition.sector_start == device .sectors
       )
    {
        //Must account the relative offset between:
        // (A) the Extended Boot Record sector and the next track of the
        //     logical partition (usually 63 sectors), and
        // (B) the Master Boot Record sector and the next track of the first
        //     primary partition
        diff = (partition .sector_start - device .sectors) % device .cylsize ;
    }
    else if ( partition.sector_start == 34 )
    {
        // (C) the GUID Partition Table (GPT) and the start of the data
        //     partition at sector 34
        diff = (partition .sector_start - 34 ) % device .cylsize ;
    }
    else
    {
        diff = partition .sector_start % device .cylsize ;
    }
    if ( diff )
    {
        if ( diff < ( device .cylsize / 2 ) || less_than_half_cylinder )
            partition .sector_start -= diff ;
        else
            partition .sector_start += (device .cylsize - diff ) ;
    }

    diff = (partition .sector_end +1) % device .cylsize ;
    if ( diff )
    {
        if ( diff < ( device .cylsize / 2 ) && ! less_than_half_cylinder )
            partition .sector_end -= diff ;
        else
            partition .sector_end += (device .cylsize - diff ) ;
    }

    return true ;
}



bool MParted::MParted_Core::snapToMebibyte(const MParted::Device & device, MParted::Partition & partition) {
    MParted::Sector diff = 0;

    if ( partition .sector_start < 2 || (partition .type == MParted::TYPE_LOGICAL && partition .freeSpaceBefore == 0) )
    {
        // Must account the relative offset between:
        // (A) the Master Boot Record sector and the first primary/extended partition, and
        // (B) the Extended Boot Record sector and the logical partition

        // Add space minimum to force alignment to next mebibyte.
        // Unless specifically told otherwise, the Linux kernel considers extended
        // boot records to be two sectors long, in order to "leave room for LILO".
        partition .sector_start += 2 ;
    }

    //Calculate difference offset from Mebibyte boundary
    diff = MParted::Sector(partition .sector_start % ( MParted::MEBIBYTE / partition .sector_size ));

    //Align start sector
    if ( diff )
    {
        partition .sector_start += ( (MParted::MEBIBYTE / partition .sector_size) - diff) ;

        //If this is an extended partition then check to see if sufficient space is
        //  available for any following logical partition Extended Boot Record
        if ( partition .type == MParted::TYPE_EXTENDED )
        {
            //Locate the extended partition that contains the logical partitions.
            int index_extended = -1 ;
            for ( int t = 0 ; t < device .partitions .size() ; t++ )
            {
                if ( device .partitions[ t ] .type == MParted::TYPE_EXTENDED )
                    index_extended = t ;
            }

            //If there is logical partition that starts less than 2 sectors
            //  from the start of this partition, then reserve a mebibyte for the EBR.
            if ( index_extended != -1 )
            {
                for ( int t = 0; t < device .partitions[ index_extended ] .logicals .size(); t++ )
                {
                    if (   ( device .partitions[ index_extended ] .logicals[ t ] .type == MParted::TYPE_LOGICAL )
                        && ( (  (  device .partitions[ index_extended ] .logicals[ t ] .sector_start )
                              - ( partition .sector_start )
                             )
                             //Unless specifically told otherwise, the Linux kernel considers extended
                             //  boot records to be two sectors long, in order to "leave room for LILO".
                             < 2
                           )
                       )
                    {
                        partition .sector_start -= (MParted::MEBIBYTE / partition .sector_size) ;
                    }
                }
            }
        }
    }

    //Align end sector
    diff = (partition .sector_end + 1) % ( MParted::MEBIBYTE / partition .sector_size);
    if ( diff )
        partition .sector_end -= diff ;

    //If this is a logical partition not at end of drive then check to see if space is
    //  required for a following logical partition Extended Boot Record
    if ( partition .type == MParted::TYPE_LOGICAL )
    {
        //Locate the extended partition that contains the logical partitions.
        int index_extended = -1 ;
        for ( int t = 0 ; t < device .partitions .size() ; t++ )
        {
            if ( device .partitions[ t ] .type == MParted::TYPE_EXTENDED )
                index_extended = t ;
        }

        //If there is a following logical partition that starts less than 2 sectors from
        //  the end of this partition, then reserve at least a mebibyte for the EBR.
        if ( index_extended != -1 )
        {
            for ( int t = 0; t < device .partitions[ index_extended ] .logicals .size(); t++ )
            {
                if (   ( device .partitions[ index_extended ] .logicals[ t ] .type == MParted::TYPE_LOGICAL )
                    && ( device .partitions[ index_extended ] .logicals[ t ] .sector_start > partition .sector_end )
                    && ( ( device .partitions[ index_extended ] .logicals[ t ] .sector_start - partition .sector_end )
                           //Unless specifically told otherwise, the Linux kernel considers extended
                           //  boot records to be two sectors long, in order to "leave room for LILO".
                         < 2
                       )
                   )
                    partition .sector_end -= ( MParted::MEBIBYTE / partition .sector_size ) ;
            }
        }
    }

    //If this is a primary or an extended partition and the partition overlaps
    //  the start of the next primary or extended partition then subtract a
    //  mebibyte from the end of the partition to address the overlap.
    if ( partition .type == MParted::TYPE_PRIMARY || partition .type == MParted::TYPE_EXTENDED )
    {
        for ( int t = 0 ; t < device .partitions .size() ; t++ )
        {
            if (   (   device .partitions[ t ] .type == MParted::TYPE_PRIMARY
                    || device .partitions[ t ] .type == MParted::TYPE_EXTENDED
                   )
                && ( device .partitions[ t ] .sector_start > partition .sector_start )
                && ( device .partitions[ t ] .sector_start <= partition .sector_end )
               )
                partition .sector_end -= ( MParted::MEBIBYTE / partition .sector_size );
        }
    }

    //If this is an extended partition then check to see if the end of the
    //  extended partition encompasses the end of the last logical partition.
    if ( partition .type == MParted::TYPE_EXTENDED )
    {
        //If there is logical partition that has an end sector beyond the
        //  end of the extended partition, then set the extended partition
        //  end sector to be the same as the end of the logical partition.
        for ( int t = 0; t < partition .logicals .size(); t++ )
        {
            if (   ( partition .logicals[ t ] .type == MParted::TYPE_LOGICAL )
                && (   (  partition .logicals[ t ] .sector_end )
                     > ( partition .sector_end )
                   )
               )
            {
                partition .sector_end = partition .logicals[ t ] .sector_end ;
            }
        }
    }

    //If this is a GPT partition table and the partition ends less than 34 sectors
    //  from the end of the device, then reserve at least a mebibyte for the
    //  backup partition table
    if (    device .diskType == "gpt"
        && ( ( device .length - partition .sector_end ) < 34 )
       )
    {
        partition .sector_end -= ( MParted::MEBIBYTE / partition .sector_size ) ;
    }

    return true;
}



//###
//### Private - Operation methods
//###



bool MParted::MParted_Core::calibratePartition(MParted::Partition & partition) {
    if (partition.type != MParted::TYPE_PRIMARY && partition.type != MParted::TYPE_LOGICAL && partition.type != MParted::TYPE_EXTENDED)
        return true; //nothing to calibrate...

    bool success = false;
    PedPartition *pedPartition = NULL;

    if (openDeviceAndDisk(partition.devicePath)) {
        if (partition.type == MParted::TYPE_EXTENDED)
            pedPartition = ped_disk_extended_partition(pedDisk);
        else
            pedPartition = ped_disk_get_partition_by_sector(pedDisk, partition.getSector());

        // Check to see if pedPartition->type matches partition.type
        if (pedPartition && ((pedPartition->type == PED_PARTITION_NORMAL && partition.type == MParted::TYPE_PRIMARY)
                   || (pedPartition->type == PED_PARTITION_LOGICAL && partition.type == MParted::TYPE_LOGICAL)
                   || (pedPartition->type == PED_PARTITION_EXTENDED && partition.type == MParted::TYPE_EXTENDED)))
        {
            // Get partition path
            partition.path = Utils::charToStringFree(ped_partition_get_path(pedPartition)); // we have to free the result of ped_partition_get_path()

            partition.sector_start = pedPartition->geom.start;
            partition.sector_end = pedPartition->geom.end;

            if (!partition.path.isEmpty())
                success = true;
        }

        closeDeviceAndDisk();
    }

    return success;
}



bool MParted::MParted_Core::removePartition(MParted::Partition & partition) {
    if (!openDeviceAndDisk(partition.devicePath))
        return false;

    PedPartition *pedPartition = NULL;

    if (partition.type == MParted::TYPE_EXTENDED)
        pedPartition = ped_disk_extended_partition(pedDisk);
    else
        pedPartition = ped_disk_get_partition_by_sector(pedDisk, partition.getSector());

    bool success = false;

    if (pedPartition != NULL)
        success = ped_disk_delete_partition(pedDisk, pedPartition) && commit();

    closeDeviceAndDisk();

    return success;
}



bool MParted::MParted_Core::removeFilesystem(MParted::Partition & partition) {
    // Find filesystem class and call the remove method
    for (int i = 0; i < filesystems.size(); i++) {
        MParted::Filesystem *fs = filesystems[i];

        if (fs->getFilesystemType() != partition.filesystem)
            continue;

        return fs->remove(partition);
    }

    // If not found return anyway true
    return true;
}



bool MParted::MParted_Core::create(const MParted::Device & device, MParted::Partition & new_partition) {
    // Perfom alignment
    if (!snapToAlignment(device, new_partition))
        return false;

    if (new_partition.type == MParted::TYPE_EXTENDED) {
        return createPartition(new_partition);
    }
    else {
        MParted::Byte_Value fsMIN = 0;
        for (int t = 0 ; t < filesystems.size(); t++) {
            if (filesystems[t]->getFilesystemType() == new_partition.filesystem) {
                fsMIN = filesystems[t]->getFilesystemMIN();
                break;
            }
        }

        if (!createPartition(new_partition, (fsMIN / new_partition.sector_size)))
            return false;

        if (new_partition.filesystem == MParted::FS_UNFORMATTED)
            return true;
        else
            return (setPartitionType(new_partition) && createFilesystem(new_partition));
    }

    return false;
}



bool MParted::MParted_Core::createPartition(MParted::Partition & new_partition, MParted::Sector min_size) {
    new_partition.partitionNumber = 0;

    if (!openDeviceAndDisk(new_partition.devicePath))
        return false;

    PedPartitionType type;
    PedPartition *pedPartition = NULL;
    PedConstraint *constraint = NULL;
    PedFileSystemType* fs_type = NULL;

    //create new partition
    switch (new_partition.type) {
        case MParted::TYPE_PRIMARY:
            type = PED_PARTITION_NORMAL;
            break;
        case MParted::TYPE_LOGICAL:
            type = PED_PARTITION_LOGICAL;
            break;
        case MParted::TYPE_EXTENDED:
            type = PED_PARTITION_EXTENDED;
            break;

        default:
            type = PED_PARTITION_FREESPACE;
    }

    if (new_partition.type != MParted::TYPE_EXTENDED)
        fs_type = ped_file_system_type_get("ext2");

    pedPartition = ped_partition_new(pedDisk, type, fs_type,
                                     new_partition.sector_start,
                                     new_partition.sector_end);

    if (!pedPartition) {
        // Clean up
        closeDeviceAndDisk();
        return false;
    }

    if (new_partition.alignment == MParted::ALIGN_MEBIBYTE) {
        PedGeometry *geom = ped_geometry_new(pedDevice,
                                             new_partition.sector_start,
                                             new_partition.getSectorLength());
        if (geom)
            constraint = ped_constraint_exact(geom);
    }
    else {
        constraint = ped_constraint_any(pedDevice);
    }


    if (constraint) {
        if (min_size > 0
                && new_partition.filesystem != MParted::FS_XFS) // Permit copying to smaller xfs partition
            constraint->min_size = min_size;

        if (ped_disk_add_partition(pedDisk, pedPartition, constraint) && commit()) {
            // Get partition path
            new_partition.path = Utils::charToStringFree(ped_partition_get_path(pedPartition)); // we have to free the result of ped_partition_get_path()

            new_partition.partitionNumber = pedPartition->num;
            new_partition.sector_start = pedPartition->geom.start;
            new_partition.sector_end = pedPartition->geom.end;
        }

        ped_constraint_destroy(constraint);
    }

    // Clean up
    closeDeviceAndDisk();


    return (new_partition.partitionNumber > 0);
}



bool MParted::MParted_Core::createFilesystem(MParted::Partition & partition) {
    MParted::Filesystem *fs = NULL;

    for (int t = 0 ; t < filesystems.size(); t++) {
        if (filesystems[t]->getFilesystemType() == partition.filesystem) {
            fs = filesystems[t];
            break;
        }
    }

    if (fs == NULL || fs->getFilesystemSupport().create != MParted::FSSupport::EXTERNAL)
        return false;

    return fs->create(partition);
}



bool MParted::MParted_Core::setPartitionType(MParted::Partition & partition) {
    if (!openDeviceAndDisk(partition.devicePath))
        return false;

    bool return_value = false;

    //Lookup libparted file system type using MParted's name, as most match
    PedFileSystemType * fs_type = ped_file_system_type_get(partition.getFilesystemString().toStdString().c_str());

    //If not found, and FS is linux-swap, then try linux-swap(v1)
    if (!fs_type && partition.getFilesystemString() == "linux-swap")
        fs_type = ped_file_system_type_get("linux-swap(v1)");

    //If not found, and FS is linux-swap, then try linux-swap(new)
    if (!fs_type && partition.getFilesystemString() == "linux-swap")
        fs_type = ped_file_system_type_get("linux-swap(new)");

    //default is Linux (83)
    if (!fs_type)
        fs_type = ped_file_system_type_get("ext2");

    if (fs_type && partition.filesystem != MParted::FS_LVM2_PV) {
        PedPartition *pedPartition = ped_disk_get_partition_by_sector(pedDisk, partition.getSector());

        //Also clear any libparted LVM flag so that it doesn't override the file system type
        if (pedPartition
                && ped_partition_set_flag(pedPartition, PED_PARTITION_LVM, 0)
                && ped_partition_set_system(pedPartition, fs_type)
                && commit())
        return_value = true;
    }
    else if (partition.filesystem == MParted::FS_LVM2_PV) {
        PedPartition *pedPartition = ped_disk_get_partition_by_sector(pedDisk, partition.getSector());

        if (pedPartition
                && ped_partition_set_flag(pedPartition, PED_PARTITION_LVM, 1)
                && commit())
        return_value = true;
    }


    closeDeviceAndDisk();

    return return_value;
}



bool MParted::MParted_Core::resize(const MParted::Device & device,
                MParted::Partition & partition_old,
                MParted::Partition & partition_new)
{
    // We only support resizing
    if (partition_new.sector_start != partition_old.sector_start)
        return false;

    // Perfom alignment
    if (!snapToAlignment(device, partition_new))
        return false;

    if (partition_new.alignment == MParted::ALIGN_CYLINDER
            && !calculateExactGeom(partition_old, partition_new))
        return false;

    // If start sector has changed then undo the change
    if (partition_new.sector_start != partition_old.sector_start)
        partition_new.sector_start = partition_old.sector_start; // TODO: is this ok? -> alignment?

    if (partition_old.type == MParted::TYPE_EXTENDED)
        return resizePartition(partition_old, partition_new);


    // First check the filesystem
    if (!checkRepairFilesystem(partition_new))
        return false;

    bool success = true;

    if (partition_new.getSectorLength() < partition_old.getSectorLength())
        success = resizeFilesystem(partition_old, partition_new);

    if (success)
        success = resizePartition(partition_old, partition_new);

    // Do not maximize file system if new size <= old. It has been resized already...
    if (success && partition_new.getSectorLength() > partition_old.getSectorLength())
        // Expand file system to fit exactly in partition
        success = maximizeFilesystem(partition_new);

    // Finally check the filesystem again
    if (!checkRepairFilesystem(partition_new))
        return false;

    return success;
}



bool MParted::MParted_Core::calculateExactGeom(MParted::Partition & partition_old, MParted::Partition & partition_new) {
    bool success = false;

    if (!openDeviceAndDisk(partition_old.devicePath))
        return false;

    PedPartition *pedPartition = NULL;

    if (partition_old.type == MParted::TYPE_EXTENDED)
        pedPartition = ped_disk_extended_partition(pedDisk);
    else
        pedPartition = ped_disk_get_partition_by_sector(pedDisk, partition_old.getSector());

    if (pedPartition) {
        PedConstraint *constraint = NULL;
        constraint = ped_constraint_any(pedDevice);

        if (constraint) {
            if (ped_disk_set_partition_geom(pedDisk,
                              pedPartition,
                              constraint,
                              partition_new.sector_start,
                              partition_new.sector_end))
            {
                partition_new.sector_start = pedPartition->geom.start;
                partition_new.sector_end = pedPartition->geom.end;
                success = true;
            }

            ped_constraint_destroy(constraint);
        }
    }

    closeDeviceAndDisk() ;

    return success;
}



bool MParted::MParted_Core::resizePartition(MParted::Partition & partition_old, MParted::Partition & partition_new) {
    if (partition_new.getSectorLength() == partition_old.getSectorLength()
            && partition_new.sector_start == partition_old.sector_start)
        return true; // New and old partition have the same size and position. Hence skipping this operation

    bool return_value = false;

    PedConstraint *constraint = NULL;
    PedPartition *pedPartition = NULL;

    if (!openDeviceAndDisk(partition_old.devicePath))
        return false;


    if (partition_old.type == MParted::TYPE_EXTENDED)
        pedPartition = ped_disk_extended_partition(pedDisk);
    else
        pedPartition = ped_disk_get_partition_by_sector(pedDisk, partition_old.getSector());

    if (pedPartition) {
        if (partition_new.alignment == MParted::ALIGN_MEBIBYTE) {
            PedGeometry *geom = ped_geometry_new(pedDevice,
                                  partition_new.sector_start,
                                  partition_new.getSectorLength());
            constraint = ped_constraint_exact(geom);
        }
        else {
            constraint = ped_constraint_any(pedDevice);
        }

        if (constraint) {
            if (ped_disk_set_partition_geom(pedDisk,
                              pedPartition,
                              constraint,
                              partition_new.sector_start,
                              partition_new.sector_end)
                    && commit())
            {
                partition_new.sector_start = pedPartition->geom.start;
                partition_new.sector_end = pedPartition->geom.end;

                return_value = true;
            }

            ped_constraint_destroy(constraint);
        }
    }

    closeDeviceAndDisk();

    return return_value;
}



bool MParted::MParted_Core::maximizeFilesystem(MParted::Partition & partition) {
    return resizeFilesystem(partition, partition, true);
}



bool MParted::MParted_Core::resizeFilesystem(MParted::Partition & partition_old, MParted::Partition & partition_new, bool fill_partition) {
    if (!fill_partition && partition_new.getSectorLength() == partition_old.getSectorLength())
        return true; // New and old file system have the same size. Hence skipping this operation

    MParted::Filesystem *fs = NULL;

    for (int t = 0 ; t < filesystems.size(); t++) {
        if (filesystems[t]->getFilesystemType() == partition_old.filesystem) {
            fs = filesystems[t];
            break;
        }
    }

    if (fs == NULL)
        return false;


    // By default grow
    MParted::FSSupport::Support action = fs->getFilesystemSupport().grow;

    if (!fill_partition && partition_new.getSectorLength() < partition_old.getSectorLength())
        action = fs->getFilesystemSupport().shrink;

    if (action != MParted::FSSupport::EXTERNAL)
        return false;

    return fs->resize(partition_new, fill_partition);
}



bool MParted::MParted_Core::checkRepairFilesystem(MParted::Partition & partition) {
    MParted::Filesystem *fs = NULL;

    for (int t = 0 ; t < filesystems.size(); t++) {
        if (filesystems[t]->getFilesystemType() == partition.filesystem) {
            fs = filesystems[t];
            break;
        }
    }

    if (fs == NULL)
        return false;

    if (fs->getFilesystemSupport().check == MParted::FSSupport::NONE)
        return true; // Checking is not available for this file system
    else if (fs->getFilesystemSupport().check != MParted::FSSupport::EXTERNAL)
        return false;

    return fs->checkRepair(partition);
}
