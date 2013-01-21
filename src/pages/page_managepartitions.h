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

#ifndef PAGE_MANAGEPARTITIONS_H
#define PAGE_MANAGEPARTITIONS_H

#include <MaWiz/mawizpage.h>
#include <QIcon>
#include <QPainter>
#include <QColor>
#include <QPixmap>
#include <QBrush>
#include <QTreeWidgetItem>
#include <QLinearGradient>
#include <QScrollBar>
#include "partitionmanager/mparted.h"
#include "widgets/partitioncreatedialog.h"


namespace Ui {
class Page_ManagePartitions;
}

class Page_ManagePartitions : public MaWizPage
{
    Q_OBJECT
    
public:
    explicit Page_ManagePartitions();
    ~Page_ManagePartitions();

    void init();
    void activated();
    
private:
    Ui::Page_ManagePartitions *ui;
    MParted::MParted_Virtual mparted;
    MParted::Partitions treeWidgetPartitions;
    PartitionCreateDialog partitionCreateDialog;

    void refreshPartitionView();
    QTreeWidgetItem* addPartitionItem(MParted::Partition & partition, QTreeWidgetItem *parent);

private slots:
    void buttonDeletePartition_clicked();
    void buttonCreatePartition_clicked();
    void buttonUnmount_clicked();
    void createNewPartition(MParted::Partition unallocatedPartition, unsigned long MBytesBefore,
                            unsigned long MBytesSize, MParted::FILESYSTEM filesystem,
                            bool asExtended, MParted::PartitionAlignment alignment, QString label);

};

#endif // PAGE_MANAGEPARTITIONS_H
