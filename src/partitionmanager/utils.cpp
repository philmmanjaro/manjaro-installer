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

#include "utils.h"



QString MParted::Utils::charToString(const char *c) {
    if (c == NULL)
        return "";

    return QString::fromLocal8Bit(c);
}



QString MParted::Utils::charToStringFree(char *c) {
    QString str = charToString(c);
    free(c);
    return str;
}



QString MParted::Utils::getFormattedSize(MParted::Sector sectors, MParted::Byte_Value sector_size) {
    std::stringstream ss;
    ss << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) ;

    if ( (sectors * sector_size) < KIBIBYTE )
    {
        ss << Utils::sectorToUnit( sectors, sector_size, UNIT_BYTE ) ;
        return QObject::tr("%1 B").arg(QString::fromStdString(ss.str()));
    }
    else if ( (sectors * sector_size) < MEBIBYTE )
    {
        ss << Utils::sectorToUnit( sectors, sector_size, UNIT_KIB ) ;
        return QObject::tr("%1 KiB").arg(QString::fromStdString(ss.str()));
    }
    else if ( (sectors * sector_size) < GIBIBYTE )
    {
        ss << Utils::sectorToUnit( sectors, sector_size, UNIT_MIB ) ;
        return QObject::tr("%1 MiB").arg(QString::fromStdString(ss.str()));
    }
    else if ( (sectors * sector_size) < TEBIBYTE )
    {
        ss << Utils::sectorToUnit( sectors, sector_size, UNIT_GIB ) ;
        return QObject::tr("%1 GiB").arg(QString::fromStdString(ss.str()));
    }
    else
    {
        ss << Utils::sectorToUnit( sectors, sector_size, UNIT_TIB ) ;
        return QObject::tr("%1 TiB").arg(QString::fromStdString(ss.str()));
    }
}



double MParted::Utils::sectorToUnit(Sector sectors, Byte_Value sector_size, SIZE_UNIT size_unit) {
    switch ( size_unit )
    {
        case UNIT_BYTE	:
            return sectors * sector_size ;

        case UNIT_KIB	:
            return sectors / ( static_cast<double>( KIBIBYTE ) / sector_size );
        case UNIT_MIB	:
            return sectors / ( static_cast<double>( MEBIBYTE ) / sector_size );
        case UNIT_GIB	:
            return sectors / ( static_cast<double>( GIBIBYTE ) / sector_size );
        case UNIT_TIB	:
            return sectors / ( static_cast<double>( TEBIBYTE ) / sector_size );

        default:
            return sectors ;
    }
}



QString MParted::Utils::getFilesystemString(MParted::FILESYSTEM filesystem) {
    switch( filesystem )
    {
        case MParted::FS_UNALLOCATED	: return
                /* TO TRANSLATORS:  unallocated
                 * means that this space on the disk device does
                 * not contain a recognized file system, and is in
                 * other words unallocated.
                 */
                QObject::tr("unallocated") ;
        case MParted::FS_UNKNOWN		: return
                /* TO TRANSLATORS:  unknown
                 * means that this space within this partition does
                 * not contain a file system known, and
                 * is in other words unknown.
                 */
                QObject::tr("unknown") ;
        case MParted::FS_UNFORMATTED	: return
                /* TO TRANSLATORS:  unformatted
                 * means that the space within this partition will not
                 * be formatted with a known file system.
                 */
                QObject::tr("unformatted") ;
        case MParted::FS_EXTENDED	: return "extended" ;
        case MParted::FS_BTRFS		: return "btrfs" ;
        case MParted::FS_EXT2		: return "ext2" ;
        case MParted::FS_EXT3		: return "ext3" ;
        case MParted::FS_EXT4		: return "ext4" ;
        case MParted::FS_LINUX_SWAP	: return "linux-swap" ;
        case MParted::FS_FAT16		: return "fat16" ;
        case MParted::FS_FAT32		: return "fat32" ;
        case MParted::FS_EXFAT		: return "exfat" ;
        case MParted::FS_NILFS2		: return "nilfs2" ;
        case MParted::FS_NTFS		: return "ntfs" ;
        case MParted::FS_REISERFS	: return "reiserfs" ;
        case MParted::FS_REISER4		: return "reiser4" ;
        case MParted::FS_XFS		: return "xfs" ;
        case MParted::FS_JFS		: return "jfs" ;
        case MParted::FS_HFS		: return "hfs" ;
        case MParted::FS_HFSPLUS		: return "hfs+" ;
        case MParted::FS_UFS		: return "ufs" ;
        case MParted::FS_USED		: return QObject::tr("used") ;
        case MParted::FS_UNUSED		: return QObject::tr("unused") ;
        case MParted::FS_LVM2_PV		: return "lvm2 pv" ;
        case MParted::FS_LUKS		: return "crypt-luks" ;

        default			: return "" ;
    }
}



QString MParted::Utils::getFilesystemColorCode(MParted::FILESYSTEM filesystem) {
    switch(filesystem)
    {
        case MParted::FS_UNALLOCATED	: return "#7A7A7A" ;	// ~ medium grey
        case MParted::FS_UNKNOWN		: return "#000000" ;	//black
        case MParted::FS_UNFORMATTED	: return "#000000" ;	//black
        case MParted::FS_EXTENDED	: return "#02E0E4" ;	// ~ Turquoise
        case MParted::FS_BTRFS		: return "#FF9955" ;	//orange
        case MParted::FS_EXT2		: return "#6992BA" ;	//blue hilight
        case MParted::FS_EXT3		: return "#7590AE" ;	//blue medium
        case MParted::FS_EXT4		: return "#4B6983" ;	//blue dark
        case MParted::FS_LINUX_SWAP	: return "#C1665A" ;	//red medium
        case MParted::FS_FAT16		: return "#00FF00" ;	//green
        case MParted::FS_FAT32		: return "#18D918" ;	// ~ medium green
        case MParted::FS_EXFAT		: return "#2E8B57" ;	// ~ sea green
        case MParted::FS_NILFS2		: return "#826647" ;	//face skin dark
        case MParted::FS_NTFS		: return "#42E5AC" ;	// ~ light turquoise
        case MParted::FS_REISERFS	: return "#ADA7C8" ;	//purple hilight
        case MParted::FS_REISER4		: return "#887FA3" ;	//purple medium
        case MParted::FS_XFS			: return "#EED680" ;	//accent yellow
        case MParted::FS_JFS			: return "#E0C39E" ;	//face skin medium
        case MParted::FS_HFS			: return "#E0B6AF" ;	//red hilight
        case MParted::FS_HFSPLUS		: return "#C0A39E" ;	// ~ serene red
        case MParted::FS_UFS			: return "#D1940C" ;	//accent yellow dark
        case MParted::FS_USED		: return "#F8F8BA" ;	// ~ light tan yellow
        case MParted::FS_UNUSED		: return "#FFFFFF" ;	//white
        case MParted::FS_LVM2_PV		: return "#CC9966" ;	// ~ medium brown
        case MParted::FS_LUKS		: return "#625B81" ;	//purple dark

        default				: return "#000000" ;
    }
}



int MParted::Utils::executeCommand(const QString & command, const QStringList & arguments) {
    QString output, error;
    return executeCommand(command, arguments, output, error);
}



int MParted::Utils::executeCommand(const QString & command, const QStringList & arguments, QString & output, QString & error) {
    output.clear();
    error.clear();

    // Get PATH environment variable
    QString PATH = charToString(getenv("PATH"));

    // Setup environment
    QStringList ENV;
    ENV.append("LC_ALL=C");

    if (!PATH.isEmpty())
        ENV.append("PATH=" + PATH);

    QProcess process;
    process.setEnvironment(ENV);
    process.start(command, arguments);

    if (!process.waitForStarted(-1))
        return -1;

    if (!process.waitForFinished(-1))
        return -1;

    output = QString(process.readAllStandardOutput());
    error = QString(process.readAllStandardError());

    return process.exitCode();
}



QString MParted::Utils::findProgramInPath(QString program) {
    // Trim program string
    program = program.trimmed();

    // Program is located in current path
    if (QFile::exists(program))
        return program;

    // Get PATH environment variable
    QStringList PATH = charToString(getenv("PATH")).split(':', QString::SkipEmptyParts);

    // Check PATH variable
    for (int i = 0; i < PATH.size(); i++) {
        if (!QFile::exists(PATH.at(i) + "/" + program))
            continue;

        return QString(PATH.at(i) + "/" + program);
    }

    // Not found
    return "";
}



MParted::Sector MParted::Utils::round(double double_value) {
     return static_cast<MParted::Sector>(double_value + 0.5);
}



QString MParted::Utils::getRegExp(const QString &str, QString pattern, bool splitNewLines) {
    QRegExp rx(pattern, Qt::CaseInsensitive);

    if (splitNewLines) {
        QStringList lines = str.split("\n", QString::SkipEmptyParts);

        for (int i = 0; i < lines.size(); i++) {
            if (rx.indexIn(lines.at(i).trimmed()) > -1)
                    return rx.cap(1).trimmed();
        }
    }
    else if (rx.indexIn(str) > -1) {
        return rx.cap(1).trimmed();
    }

    return "";
}

