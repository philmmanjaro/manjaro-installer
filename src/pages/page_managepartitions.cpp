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

#include "page_managepartitions.h"
#include "ui_page_managepartitions.h"

Page_ManagePartitions::Page_ManagePartitions() :
    MaWizPage(),
    ui(new Ui::Page_ManagePartitions)
{
    ui->setupUi(this);
    setTitle(tr("Prepare Partitions"));
    setHelpURL("http://wiki.manjaro.org");

    // Set up MParted_Virtual and partition widgets
    mparted = new MParted::MParted_Virtual();
    partitionCreateDialog = new PartitionCreateDialog(mparted, this);

    // Set emtpy text of lock header item
    if (ui->PartitionView->headerItem() != NULL)
        ui->PartitionView->headerItem()->setText(2, "");

    // Set size of header items
    ui->PartitionView->setColumnWidth(0, 200);
    ui->PartitionView->setColumnWidth(1, 120);
    ui->PartitionView->setColumnWidth(2, 30);
    ui->PartitionView->setColumnWidth(3, 105);
    ui->PartitionView->setColumnWidth(4, 105);
    ui->PartitionView->setColumnWidth(5, 120);
    ui->PartitionView->setColumnWidth(6, 50);

    // Connect signals and slots
    connect(ui->buttonDeletePartition, SIGNAL(clicked())    ,   this, SLOT(buttonDeletePartition_clicked()));
    connect(ui->buttonCreatePartition, SIGNAL(clicked())    ,   this, SLOT(buttonCreatePartition_clicked()));
    connect(ui->buttonUnmount, SIGNAL(clicked())    ,   this, SLOT(buttonUnmount_clicked()));
    connect(partitionCreateDialog, SIGNAL(requestNewPartition(MParted::Partition,ulong,ulong,MParted::FILESYSTEM,bool,MParted::PartitionAlignment,QString)),
            this, SLOT(createNewPartition(MParted::Partition,ulong,ulong,MParted::FILESYSTEM,bool,MParted::PartitionAlignment,QString)));
}



Page_ManagePartitions::~Page_ManagePartitions()
{
    delete partitionCreateDialog;
    delete mparted;
    delete ui;

    // Release MParted library
    MParted::MParted_Core::release();
}



void Page_ManagePartitions::init() {
    // Init MParted library
    MParted::MParted_Core::init();
}



void Page_ManagePartitions::activated() {
    mparted->scanAndReset();
    refreshPartitionView();
}



//###
//### Private
//###


void Page_ManagePartitions::refreshPartitionView() {
    // Save vertical scroll bar value
    int scrollYValue = 0;
    QScrollBar *scrollBar = ui->PartitionView->verticalScrollBar();

    if (scrollBar)
        scrollYValue = scrollBar->value();

    // Clean up
    ui->PartitionView->clear();


    MParted::Devices devices = mparted->getDevices();
    QFont font;
    font.setBold(true);
    font.setWeight(75);

    for (int i = 0; i < devices.size(); i++) {
        MParted::Device &device = devices[i];
        QTreeWidgetItem *itemDevice = new QTreeWidgetItem();
        ui->PartitionView->addTopLevelItem(itemDevice);
        ui->PartitionView->setFirstItemColumnSpanned(itemDevice, true);

        // Set text
        QString deviceText = QString("%1 (%2, %3)").arg(device.path, device.model, device.getFormattedSize());

        if (device.unkownPartitionTable)
            deviceText += "  -  " + tr("unkown Partition Table");

        itemDevice->setText(0, deviceText);

        // Set icon
        if (device.removable)
            itemDevice->setIcon(0, QIcon(":/images/resources/removabledisk.png"));
        else
            itemDevice->setIcon(0, QIcon(":/images/resources/disk.png"));


        // Partitions
        for (int x = 0; x < device.partitions.size(); x++) {
            MParted::Partition &partition = device.partitions[x];
            QTreeWidgetItem *itemPartition = addPartitionItem(partition, itemDevice);

            // Logical partitions if present
            if (!partition.logicals.isEmpty()) {
                for (int x = 0; x < partition.logicals.size(); ++x) {
                    addPartitionItem(partition.logicals[x], itemPartition);
                }
            }
        }

        // Set up itemDevice
        itemDevice->setFont(0, font);
        itemDevice->setSizeHint(0, QSize(0, 32));
        itemDevice->setExpanded(true);
        itemDevice->setFlags(Qt::ItemIsEnabled);
    }

    // Be sure the tree widget list is filled already and that the scrollbar is visible already
    qApp->processEvents();

    // Restore vertical scroll bar value
    scrollBar = ui->PartitionView->verticalScrollBar();

    if (scrollBar && scrollYValue > 0) {
        if (scrollYValue > scrollBar->maximum())
            scrollYValue = scrollBar->maximum();

        scrollBar->setSliderPosition(scrollYValue);
    }
}



QTreeWidgetItem* Page_ManagePartitions::addPartitionItem(MParted::Partition & partition, QTreeWidgetItem *parent) {
    TreeWidgetPartitionItem *item = new TreeWidgetPartitionItem();
    parent->addChild(item);
    item->setExpanded(true);
    item->setSizeHint(0, QSize(0, 32));

    // Set item's partition
    item->partition = partition;

    // Filesystem color icon
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);

    // Gradient style
    QColor color = partition.getFilesystemColor();
    QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0.0, color.lighter());
    gradient.setColorAt(1.0, color.darker());
    painter.setBrush(gradient);

    // Fill rect
    painter.fillRect(0, 0, 16, 16, gradient);
    painter.end();

    // Add text and icons
    item->setText(0, partition.path);
    item->setText(1, partition.getFilesystemString());
    item->setIcon(1, QIcon(pixmap));
    item->setText(3, partition.getFormattedSize());
    item->setText(4, partition.label);

    if (partition.busy)
        item->setIcon(2, QIcon(":/images/resources/lock_small.png"));

    return item;
}



//###
//### Private slots
//###


void Page_ManagePartitions::buttonDeletePartition_clicked() {
    // TODO error handling!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TreeWidgetPartitionItem *item = dynamic_cast<TreeWidgetPartitionItem*>(ui->PartitionView->currentItem());
    if (!item) {
        // TODO: error message!
        return;
    }

    if (!mparted->removePartition(item->partition)) {
        // TODO: messagebox
        return;
    }

    // Success -> refresh view
    refreshPartitionView();
}



void Page_ManagePartitions::buttonCreatePartition_clicked() {
    // TODO error handling!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TreeWidgetPartitionItem *item = dynamic_cast<TreeWidgetPartitionItem*>(ui->PartitionView->currentItem());
    if (!item) {
        // TODO: error message!
        return;
    }

    partitionCreateDialog->setData(item->partition);
    partitionCreateDialog->exec();
}



void Page_ManagePartitions::buttonUnmount_clicked() {
    if (!mparted->applyToDisk())
        qDebug("error!");
    else
        qDebug("success!");
}


// TODO: put this into the widget class
void Page_ManagePartitions::createNewPartition(MParted::Partition unallocatedPartition, unsigned long MBytesBefore,
                        unsigned long MBytesSize, MParted::FILESYSTEM filesystem,
                        bool asExtended, MParted::PartitionAlignment alignment, QString label) {
    if (!mparted->createPartition(unallocatedPartition, MBytesBefore, MBytesSize, filesystem, asExtended, alignment, label)) {
        // TODO: messagebox
    }
    else {
        // Success -> refresh view
        refreshPartitionView();
    }
}
