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

#include "mparted_virtual.h"

MParted::MParted_Virtual::MParted_Virtual()
{
}



MParted::MParted_Virtual::~MParted_Virtual() {
    qDeleteAll(operations);
    operations.clear();
}



void MParted::MParted_Virtual::scanAndReset() {
    QMutexLocker locker(&mutex);

    // Clean up first
    devices.clear();
    devices_orig.clear();
    qDeleteAll(operations);
    operations.clear();

    // Scan devices
    MParted_Core::scan(devices);

    // Save devices to backup
    devices_orig = devices;
}



bool MParted::MParted_Virtual::applyToDisk() {
    QMutexLocker locker(&mutex);

    for (int i = 0; i < operations.size(); i++) {
        if (!MParted::MParted_Core::applyOperationToDisk(operations[i])) {
            scanAndReset();
            return false;
        }
    }
//TODO clear!
    return true;
}



bool MParted::MParted_Virtual::isExtendedPartitionCreatable(const MParted::Partition & unallocatedPartition) {
    if (unallocatedPartition.insideExtended)
        return false;

    QMutexLocker locker(&mutex);

    // Find device
    int index = findDeviceIndex(unallocatedPartition);
    if (index < 0)
        return false;

    MParted::Device &device = devices[index];

    // Check if it is possible to create another primary partition
    int count = 0;
    for (int i = 0; i < device.partitions.size(); i++)
        if (device.partitions.at(i).type == MParted::TYPE_PRIMARY || device.partitions.at(i).type == MParted::TYPE_EXTENDED)
            ++count;

    if (count >= device.maxPrimaryPartitions)
        return false;

    // Check if there is already an extended partition
    return !device.hasExtendedPartition();
}



bool MParted::MParted_Virtual::isPartitionCreatable(const MParted::Partition & unallocatedPartition) {
    // Partition has to be an unallocated partition with a minimum size of 1MB
    if (unallocatedPartition.filesystem != MParted::FS_UNALLOCATED
            || Utils::round(Utils::sectorToUnit(unallocatedPartition.getSectorLength(), unallocatedPartition.sector_size, MParted::UNIT_MIB)) < 1)
        return false;

    QMutexLocker locker(&mutex);

    // Find device
    int index = findDeviceIndex(unallocatedPartition);
    if (index < 0)
        return false;

    MParted::Device &device = devices[index];


    // If this is a primary partition then check if it is possible to create another primary partition
    if (!unallocatedPartition.insideExtended) {
        int count = 0;
        for (int i = 0; i < device.partitions.size(); i++)
            if (device.partitions.at(i).type == MParted::TYPE_PRIMARY || device.partitions.at(i).type == MParted::TYPE_EXTENDED)
                ++count;

        if (count >= device.maxPrimaryPartitions)
            return false;
    }

    return true;
}



MParted::MParted_Virtual::PARTITION_CREATE_RANGES MParted::MParted_Virtual::getCreatePartitionRanges(const MParted::Partition & unallocatedPartition, MParted::FILESYSTEM filesystem) {
    MParted::MParted_Virtual::PARTITION_CREATE_RANGES values;
    MParted::Byte_Value fsMIN = 0, fsMAX = 0;
    MParted::Sector TOTAL_MB = Utils::round(Utils::sectorToUnit(unallocatedPartition.getSectorLength(), unallocatedPartition.sector_size, MParted::UNIT_MIB));
    values.MIN_SPACE_BEFORE_MB = MBNeededForBootRecord(unallocatedPartition);

    // Find filesystem class and save fsMIN and fsMAX
    for (int i = 0; i < MParted::MParted_Core::filesystems.size(); i++) {
        MParted::Filesystem *fs = MParted::MParted_Core::filesystems[i];

        if (fs->getFilesystemType() != filesystem)
            continue;

        fsMIN = fs->getFilesystemMIN();
        fsMAX = fs->getFilesystemMAX();
        break;
    }

    if (fsMIN < MParted::MEBIBYTE)
        fsMIN = MParted::MEBIBYTE;

    if (unallocatedPartition.getByteLength() < fsMIN)
        fsMIN = unallocatedPartition.getByteLength() ;

    if (!fsMAX || (fsMAX > ((TOTAL_MB - (int)values.MIN_SPACE_BEFORE_MB) * MParted::MEBIBYTE)))
        fsMAX = ((TOTAL_MB - values.MIN_SPACE_BEFORE_MB) * MParted::MEBIBYTE);

    // Set new ranges
    values.MIN_FS_SIZE_MB = ceil(fsMIN / double(MParted::MEBIBYTE));
    values.MAX_FS_SIZE_MB = ceil(fsMAX / double(MParted::MEBIBYTE));
    values.MAX_SPACE_BEFORE_MB = TOTAL_MB - ceil(fsMIN / double(MParted::MEBIBYTE));
    values.MIN_SPACE_AFTER_MB = 0;
    values.MAX_SPACE_AFTER_MB = TOTAL_MB - values.MIN_SPACE_BEFORE_MB - ceil(fsMIN / double(MParted::MEBIBYTE));

    return values;
}



bool MParted::MParted_Virtual::removePartition(MParted::Partition & partition) {
    QMutexLocker locker(&mutex);

    // If partition is unallocated or it is an extended partition and there still exist logical partitions, then abort!
    // TODO: Check is mounted!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"!"!!!!!!!!!!!!!!!!!!!!!
    if (partition.filesystem == MParted::FS_UNALLOCATED || partition.hasLogicalPartitions() || partition.busy)
        return false;

    // Find device
    int index = findDeviceIndex(partition);
    if (index < 0)
        return false;

    MParted::Device &device = devices[index];


    /* since logicals are *always* numbered from 5 to <last logical> there can be a shift
     * in numbers after deletion.
     * e.g. consider /dev/hda5 /dev/hda6 /dev/hda7. Now after removal of /dev/hda6,
     * /dev/hda7 is renumbered to /dev/hda6
     * the new situation is now /dev/hda5 /dev/hda6. If /dev/hda7 was mounted
     * the OS cannot find /dev/hda7 anymore and the results aren't that pretty.
     * It seems best to check for this and prohibit deletion with some explanation to the user.*/
     /*if ( 	selected_partition .type == GParted::TYPE_LOGICAL &&
        selected_partition .status != GParted::STAT_NEW &&
        selected_partition .partition_number < devices[ current_device ] .highest_busy )
    {
        Gtk::MessageDialog dialog( *this,
                       String::ucompose( _( "Unable to delete %1!"), selected_partition .get_path() ),
                       false,
                       Gtk::MESSAGE_ERROR,
                       Gtk::BUTTONS_OK,
                       true ) ;

        dialog .set_secondary_text(
            String::ucompose( _("Please unmount any logical partitions having a number higher than %1"),
                      selected_partition .partition_number ) ) ;

        dialog .run() ;
        return;
    }*/
    if (partition.type == MParted::TYPE_LOGICAL && partition.status != MParted::STAT_NEW) {
        // Find logical partitions and check for highest busy
        for (int i = 0; i < device.partitions.size(); i++) {
            if (!device.partitions[i].hasLogicalPartitions())
                continue;

            MParted::Partitions logicals = device.partitions[i].logicals;
            int highestBusy = -1;

            for (int x = 0; x < logicals.size(); x++) {
                if (logicals[x].busy && logicals[x].partitionNumber > highestBusy)
                    highestBusy = logicals[x].partitionNumber;
            }

            if (partition.partitionNumber < highestBusy)
                return false;

            break;
        }
    }


    // if deleted one is NEW, it doesn't make sense to add it to the operationslist,
    // we erase its creation and possible modifications like resize etc.. from the operationslist.
    if (partition.status == MParted::STAT_NEW) {
        //remove all operations done on this new partition (this includes creation)
        QList<MParted::Operation*>::iterator iter = operations.begin();
        while (iter != operations.end()) {
            if ((*iter)->partition_new.path == partition.path)
                iter = operations.erase(iter);
            else
                ++iter;
        }

        return rebuildVisual();
    }
    else {
        // Add Operation
        return addOperation(new MParted::OperationRemove(device, partition), device);
    }
}



bool MParted::MParted_Virtual::createPartition(MParted::Partition & unallocatedPartition,
                                               unsigned long MBytesBefore, unsigned long MBytesSize,
                                               MParted::FILESYSTEM filesystem, bool asExtended,
                                               MParted::PartitionAlignment alignment, QString label) {
    QMutexLocker locker(&mutex);

    // Basic checks
    if (unallocatedPartition.filesystem != MParted::FS_UNALLOCATED || (unallocatedPartition.insideExtended && asExtended))
        return false;


    // Check if ranges of new partition are ok
    PARTITION_CREATE_RANGES ranges = getCreatePartitionRanges(unallocatedPartition, filesystem);

    if (MBytesSize < ranges.MIN_FS_SIZE_MB
            || MBytesSize > ranges.MAX_FS_SIZE_MB
            || MBytesBefore < ranges.MIN_SPACE_BEFORE_MB
            || MBytesBefore > ranges.MAX_SPACE_BEFORE_MB)
        return false;


    // Find device
    int index = findDeviceIndex(unallocatedPartition);
    if (index < 0)
        return false;

    MParted::Device &device = devices[index];


    // If this is a primary partition then check if it is possible to create another primary partition
    if (!unallocatedPartition.insideExtended) {
        int count = 0;
        for (int i = 0; i < device.partitions.size(); i++)
            if (device.partitions.at(i).type == MParted::TYPE_PRIMARY || device.partitions.at(i).type == MParted::TYPE_EXTENDED)
                ++count;

        if (count >= device.maxPrimaryPartitions)
            return false;
    }

    // Check if extended partition already exists and asExtended is true
    if (asExtended && device.hasExtendedPartition())
        return false;


    // Create new Partition
    MParted::Partition partition;
    partition.status = MParted::STAT_NEW;
    partition.devicePath = device.path;
    partition.filesystem = filesystem;
    partition.sector_size = device.sector_size;
    partition.insideExtended = unallocatedPartition.insideExtended;
    partition.partitionNumber = lowestPossiblePartitionCount();
    partition.path = QObject::tr("New Partition #%1").arg(QString::number(partition.partitionNumber));
    partition.label = label.trimmed();
    partition.alignment = alignment;

    // Set partition type
    if (asExtended) {
        partition.type = MParted::TYPE_EXTENDED;
        partition.filesystem = MParted::FS_EXTENDED;
    }
    else if (unallocatedPartition.insideExtended) {
        partition.type = MParted::TYPE_LOGICAL;
    }
    else {
        partition.type = MParted::TYPE_PRIMARY;
    }


    // Calculate new sector start and end
    partition.sector_start = unallocatedPartition.sector_start + (MParted::Sector(MBytesBefore) * (MParted::MEBIBYTE / device.sector_size));
    partition.sector_end  = partition.sector_start + (MParted::Sector(MBytesSize) * (MParted::MEBIBYTE / device.sector_size)) - 1;

    /* due to loss of precision during calcs from Sector -> MiB and back, it is possible the new
     * partition thinks it's bigger then it can be. Here we try to solve this.*/
    if (partition.sector_start < unallocatedPartition.sector_start)
        partition.sector_start = unallocatedPartition.sector_start;
    if  (partition.sector_end > unallocatedPartition.sector_end)
        partition.sector_end = unallocatedPartition.sector_end;

    // Grow new partition a bit if freespaces are < 1 MiB
    if ( (partition.sector_start - unallocatedPartition.sector_start) < (MParted::MEBIBYTE / device.sector_size) )
        partition.sector_start = unallocatedPartition.sector_start;
    if ( (unallocatedPartition.sector_end - partition.sector_end) < (MParted::MEBIBYTE / device.sector_size) )
        partition.sector_end = unallocatedPartition.sector_end;

    // Set alignment
    if (partition.alignment == MParted::ALIGN_MEBIBYTE) {
        // If start sector not MiB aligned and free space available then add ~1 MiB to partition so requested size is kept
       MParted::Sector diff = (MParted::MEBIBYTE / partition.sector_size) - (partition.sector_end + 1) % (MParted::MEBIBYTE / partition.sector_size);
       if (diff && (partition.sector_start % (MParted::MEBIBYTE / partition.sector_size)) > 0
               && ((partition.sector_end - unallocatedPartition.sector_start +1 + diff ) < (unallocatedPartition.sector_end - unallocatedPartition.sector_start))) {
           partition.sector_end += diff;
       }
    }

    // Set free space preceding partition value
    partition.freeSpaceBefore = partition.sector_start - unallocatedPartition.sector_start;
    if (partition.freeSpaceBefore < 0)
        partition.freeSpaceBefore = 0;

    // Perfom alignment
    if (!MParted::MParted_Core::snapToAlignment(device, partition))
        return false;

    // Apply operation
    return addOperation(new MParted::OperationCreate(device, unallocatedPartition, partition), device);
}



//###
//### Private
//###


int MParted::MParted_Virtual::findDeviceIndex(const MParted::Partition & partition) {
    for (int t = 0; t < devices.size() ; t++)
        if (devices.at(t).path == partition.devicePath)
            return t;

    return -1;
}



int MParted::MParted_Virtual::lowestPossiblePartitionCount() {
    //determine lowest possible partition count
    int count = 0;

    for (int t = 0; t < operations.size(); t++) {
        if (operations[t]->partition_new.status == MParted::STAT_NEW &&
            operations[t]->partition_new.partitionNumber > count)
            count = operations[t]->partition_new.partitionNumber;
    }

    return count + 1;
}



bool MParted::MParted_Virtual::addOperation(MParted::Operation *operation, MParted::Device & device) {
    // Save partitions of device for fallback
    MParted::Partitions fallbackPartitions = device.partitions;

    // Apply virtual changes
    if (!operation->applyToVisual(device.partitions)) {
        // Fallback and clean up
        device.partitions = fallbackPartitions;
        delete operation;
        return false;
    }

    // Save operation to list
    operations.append(operation);

    // Verify if the two operations can be merged
    mergeOperations();

    return true;
}



bool MParted::MParted_Virtual::rebuildVisual() {
    // Verify if operations can be merged
    mergeOperations();

    // Restore backup
    devices = devices_orig;

    // Apply to visual again
    for (int i = 0; i < devices.size(); i++) {
        for (int t = 0; t < operations.size(); t++) {
            if (devices[i] != operations[t]->device)
                continue;

            if (!operations[t]->applyToVisual(devices[i].partitions)) {
                // If this happens, clean up everything
                scanAndReset();
                return false;
            }
        }
    }

    return true;
}



void MParted::MParted_Virtual::mergeOperations() {
    if (operations.isEmpty())
        return;

    // Verify if the two operations can be merged
    QList<MParted::Operation*>::iterator first = operations.begin();
    QList<MParted::Operation*>::iterator second = first + 1;

    while (second != operations.end()) {
        // Two resize operations of the same partition
        if ( (*first)->type == MParted::OPERATION_RESIZE &&
             (*second)->type == MParted::OPERATION_RESIZE &&
             (*first)->partition_new == (*second)->partition_original
           )
        {
            (*first)->partition_new = (*second)->partition_new;
            second = operations.erase(second);
            first = second - 1;
        }
        // Two label change operations on the same partition
        /*else if ( (*first)->type == MParted::OPERATION_LABEL_PARTITION &&
                  (*second)->type == MParted::OPERATION_LABEL_PARTITION &&
                  (*first)->partition_new == (*second)->partition_original
                )
        {
            (*first)->partition_new.label = (*second)->partition_new.label;
            second = operations.erase(second);
            first = second - 1;
        }*/
        // Two format operations of the same partition
        else if ( (*first)->type == MParted::OPERATION_FORMAT &&
                  (*second)->type == MParted::OPERATION_FORMAT &&
                  (*first)->partition_new == (*second)->partition_original
                )
        {
            (*first)->partition_new = (*second)->partition_new;
            second = operations.erase(second);
            first = second - 1;
        }
        else {
            ++first;
            ++second;
        }
    }
}



int MParted::MParted_Virtual::MBNeededForBootRecord(const MParted::Partition & partition) {
    //Determine if space is needed for the Master Boot Record or
    //  the Extended Boot Record.  Generally an an additional track or MEBIBYTE
    //  is required so for our purposes reserve a MEBIBYTE in front of the partition.
    if (   (   partition.insideExtended
            && partition.type == MParted::TYPE_UNALLOCATED
           )
        || ( partition.type == MParted::TYPE_LOGICAL )
                                         /* Beginning of disk device */
        || ( partition.sector_start <= (MParted::MEBIBYTE / partition.sector_size) )
       )
        return 1;
    else
        return 0;
}
