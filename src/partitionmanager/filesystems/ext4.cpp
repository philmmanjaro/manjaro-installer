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

#include "ext4.h"



MParted::ext4::ext4() {
    filesystem = MParted::FS_EXT4;

    //To be on the safe side, only enable all the function if mkfs.ext4 is
    //  found indicating that here is a recent copy of e2fsprogs available.
    if (!Utils::findProgramInPath("mkfs.ext4").isEmpty()) {
        fsSupport.create = MParted::FSSupport::EXTERNAL ;

        if (!Utils::findProgramInPath("dumpe2fs").isEmpty())
            fsSupport.read_used_sectors = MParted::FSSupport::EXTERNAL;

        if (!Utils::findProgramInPath("tune2fs").isEmpty())
            fsSupport.read_uuid = MParted::FSSupport::EXTERNAL ;

        if (!Utils::findProgramInPath("e2label").isEmpty()) {
            fsSupport.read_label = MParted::FSSupport::EXTERNAL ;
            fsSupport.write_label = MParted::FSSupport::EXTERNAL ;
        }

        if (!Utils::findProgramInPath("e2fsck").isEmpty())
            fsSupport.check = MParted::FSSupport::EXTERNAL ;

        if (!Utils::findProgramInPath("resize2fs").isEmpty() && fsSupport.check) {
            fsSupport.grow = MParted::FSSupport::EXTERNAL;

            if (fsSupport.read_used_sectors) //needed to determine a min file system size..
                fsSupport.shrink = MParted::FSSupport::EXTERNAL;
        }
    }
}



void MParted::ext4::setUsedSectors(Partition & partition) {
    if (!fsSupport.read_used_sectors || Utils::executeCommand("dumpe2fs", QStringList() << "-h" << partition.path, output, error) != 0 || output.isEmpty())
        return;

    QString work = Utils::getRegExp(output, "Block count:\\s*(.*)");
    if (work.isEmpty())
        return;

    T = work.toLongLong();


    if ((work = Utils::getRegExp(output, "Block size:\\s*(.*)")).isEmpty())
        return;

    S = work.toLongLong();


    if ((work = Utils::getRegExp(output, "Free blocks:\\s*(.*)")).isEmpty())
        return;

    N = work.toLongLong();


    T = Utils::round( T * ( S / double(partition.sector_size) ) ) ;
    N = Utils::round( N * ( S / double(partition.sector_size) ) ) ;

    if (T <= -1 || N <= -1)
        return;

    partition.setSectorUsage(T, N);
}



void MParted::ext4::readUUID(Partition & partition) {
    if (!fsSupport.read_uuid || Utils::executeCommand("tune2fs", QStringList() <<  "-l" << partition.path, output, error) != 0 || output.isEmpty())
        return;

    partition.uuid = Utils::getRegExp(output, "Filesystem UUID:\\s*(.*)");
}



void MParted::ext4::readLabel(Partition & partition) {
    if (!fsSupport.read_label || Utils::executeCommand("e2label", QStringList() << partition.path, output, error) != 0 || output.isEmpty())
        return;

    partition.label = output.trimmed();
}



bool MParted::ext4::writeLabel(Partition & partition) {
    return (fsSupport.write_label && Utils::executeCommand("e2label", QStringList() << partition.path << partition.label) == 0);
}



bool MParted::ext4::create(Partition & new_partition) {
    return (fsSupport.create && Utils::executeCommand("mkfs.ext4", QStringList() << "-j" << "-O" << "extent" << "-L" << new_partition.label << new_partition.path) == 0);
}



bool MParted::ext4::resize(Partition & partition_new, bool fill_partition) {
    QStringList args;
    args.append(partition_new.path);

    if (!fill_partition)
        args.append(QString::number(Utils::round(Utils::sectorToUnit(
                                                     partition_new.getSectorLength(), partition_new.sector_size, UNIT_KIB)) -1) + "K");

    return (Utils::executeCommand("resize2fs", args) == 0);
}



bool MParted::ext4::remove(Partition &) {
    return true;
}



bool MParted::ext4::checkRepair(Partition & partition) {
    if (!fsSupport.check)
        return false;

    int exit_status = Utils::executeCommand("e2fsck", QStringList() << "-f" << "-y" << "-v" << partition.path);

    //exitstatus 256 isn't documented, but it's returned when the 'FILE SYSTEM IS MODIFIED'
    //this is quite normal (especially after a copy) so we let the function return true...
    return (exit_status == 0 || exit_status == 1 || exit_status == 2 || exit_status == 256);
}

