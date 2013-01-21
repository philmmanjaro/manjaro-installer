/*
 *  Manjaro Installation Framework
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

#ifndef PARTITIONCREATEDIALOG_H
#define PARTITIONCREATEDIALOG_H

#include <QDialog>
#include <QStyle>
#include <QList>
#include <QString>
#include <QModelIndex>
#include <QVariant>
#include "partitionmanager/mparted.h"


namespace Ui {
class PartitionCreateDialog;
}


class PartitionCreateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PartitionCreateDialog(MParted::MParted_Virtual *mparted, QWidget *parent = 0);
    ~PartitionCreateDialog();

    void setData(MParted::Partition & unallocatedPartition);

signals:
    void requestNewPartition(MParted::Partition unallocatedPartition, unsigned long MBytesBefore,
                             unsigned long MBytesSize, MParted::FILESYSTEM filesystem,
                             bool asExtended, MParted::PartitionAlignment alignment, QString label);
    
private:
    struct Filesystem {
        MParted::FILESYSTEM fs;
        QString name;
    };

    Ui::PartitionCreateDialog *ui;
    MParted::MParted_Virtual *mparted;
    MParted::Partition unallocatedPartition;
    QList<Filesystem> supportedFilesystems;

    void enableComboBoxTypeItem(int index, bool enabled);
    MParted::FILESYSTEM getCurrentFilesystem();
    void fillFilesystemComboBox();

private slots:
    void updateRanges();
    void spinSpaceBefore_valueChanged(int value);
    void spinSpaceAfter_valueChanged(int value);
    void spinSize_valueChanged(int value);
    void buttonBox_accepted();
    void comboBoxType_indexChanged(int index);

};

#endif // PARTITIONCREATEDIALOG_H
