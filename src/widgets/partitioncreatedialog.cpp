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

#include "partitioncreatedialog.h"
#include "ui_partitioncreatedialog.h"



PartitionCreateDialog::PartitionCreateDialog(MParted::MParted_Virtual *mparted, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartitionCreateDialog)
{
    ui->setupUi(this);
    this->mparted = mparted;

    // Connect signals and slots
    connect(ui->buttonBox, SIGNAL(accepted())   ,   this, SLOT(buttonBox_accepted()));
    connect(ui->buttonBox, SIGNAL(rejected())   ,   this, SLOT(close()));
    connect(ui->spinSpaceBefore, SIGNAL(valueChanged(int))  ,   this, SLOT(spinSpaceBefore_valueChanged(int)));
    connect(ui->spinSpaceAfter, SIGNAL(valueChanged(int))   ,   this, SLOT(spinSpaceAfter_valueChanged(int)));
    connect(ui->spinSize, SIGNAL(valueChanged(int)) ,   this, SLOT(spinSize_valueChanged(int)));
    connect(ui->comboBoxSupportedFilesystems, SIGNAL(currentIndexChanged(int))  ,   this, SLOT(updateRanges()));
    connect(ui->comboBoxType, SIGNAL(currentIndexChanged(int))  ,   this, SLOT(comboBoxType_indexChanged(int)));
}



PartitionCreateDialog::~PartitionCreateDialog()
{
    delete ui;
}



void PartitionCreateDialog::setData(MParted::Partition & unallocatedPartition) {
    this->unallocatedPartition = unallocatedPartition;
    ui->comboBoxAlignment->setCurrentIndex(0);
    ui->textBoxLabel->clear();

    // Set types
    if (unallocatedPartition.insideExtended) {
        enableComboBoxTypeItem(0, false);
        enableComboBoxTypeItem(1, false);
        enableComboBoxTypeItem(2, true);
        ui->comboBoxType->setCurrentIndex(2);
    }
    else {
        if (mparted->isExtendedPartitionCreatable(unallocatedPartition))
            enableComboBoxTypeItem(1, true);
        else
            enableComboBoxTypeItem(1, false);

        enableComboBoxTypeItem(0, true);
        enableComboBoxTypeItem(2, false);
        ui->comboBoxType->setCurrentIndex(0);
    }

    // Fill supported filesystem list
    supportedFilesystems.clear();
    QList<MParted::FILESYSTEM> filesystems = MParted::MParted_Core::getSupportedFormatFilesystems();

    for (int i = 0; i < filesystems.size(); i++) {
        Filesystem filesystem;
        filesystem.fs = filesystems.at(i);
        filesystem.name = MParted::Utils::getFilesystemString(filesystem.fs);
        supportedFilesystems.append(filesystem);
    }

    fillFilesystemComboBox();
    unallocatedPartition.filesystem = MParted::FS_EXT4;
    ui->partResizerWidget->setPartition(unallocatedPartition);

    // Set to maximum size
    ui->spinSize->setValue(ui->spinSize->maximum());
}



//###
//### Private
//###


void PartitionCreateDialog::enableComboBoxTypeItem(int i, bool enabled) {
    // Get the index of the value to disable
    QModelIndex index = ui->comboBoxType->model()->index(i, 0);

    QVariant v(0);
    if (enabled)
        v = QVariant(1 | 32);

    // the magic
    ui->comboBoxType->model()->setData(index, v, Qt::UserRole - 1);
}



MParted::FILESYSTEM PartitionCreateDialog::getCurrentFilesystem() {
    // Check if extened
    if (ui->comboBoxType->currentIndex() == 1)
        return MParted::FS_EXTENDED;

    // Find filesystem
    QString currentFilesystem = ui->comboBoxSupportedFilesystems->currentText();

    for (int i = 0; i < supportedFilesystems.size(); i++) {
        if (supportedFilesystems.at(i).name == currentFilesystem)
            return supportedFilesystems.at(i).fs;
    }

    return MParted::FS_UNFORMATTED;
}



void PartitionCreateDialog::fillFilesystemComboBox() {
    ui->comboBoxSupportedFilesystems->blockSignals(true);
    ui->comboBoxSupportedFilesystems->clear();

    for (int i = 0; i < supportedFilesystems.size(); i++) {
        ui->comboBoxSupportedFilesystems->addItem(supportedFilesystems.at(i).name);
    }

    ui->comboBoxSupportedFilesystems->model()->sort(0);
    ui->comboBoxSupportedFilesystems->blockSignals(false);

    // Update ranges
    updateRanges();
}



//###
//### Private slots
//###


void PartitionCreateDialog::updateRanges() {
    MParted::MParted_Virtual::PARTITION_CREATE_RANGES ranges = mparted->getCreatePartitionRanges(unallocatedPartition, getCurrentFilesystem());

    // Set ranges
    ui->spinSpaceBefore->setRange(ranges.MIN_SPACE_BEFORE_MB, ranges.MAX_SPACE_BEFORE_MB);
    ui->spinSpaceAfter->setRange(ranges.MIN_SPACE_AFTER_MB, ranges.MAX_SPACE_AFTER_MB);
    ui->spinSize->setRange(ranges.MIN_FS_SIZE_MB, ranges.MAX_FS_SIZE_MB);

    // Check if selected size is valid
    if (ui->spinSize->value() < (int)ranges.MIN_FS_SIZE_MB)
        ui->spinSize->setValue(ranges.MIN_FS_SIZE_MB);
    else if (ui->spinSize->value() > (int)ranges.MAX_FS_SIZE_MB)
        ui->spinSize->setValue(ranges.MAX_FS_SIZE_MB);
    else
        spinSize_valueChanged(ui->spinSize->value()); // Just to be sure the other spin buttons are set right

    // Update labels
    ui->labelMinimumSize->setText(QString("%1 MiB").arg(QString::number(ranges.MIN_FS_SIZE_MB)));
    ui->labelMaximumSize->setText(QString("%1 MiB").arg(QString::number(ranges.MAX_FS_SIZE_MB)));
}



void PartitionCreateDialog::spinSpaceBefore_valueChanged(int value) {
    ui->spinSpaceAfter->blockSignals(true);
    ui->spinSize->blockSignals(true);

    ui->spinSpaceAfter->setValue(ui->spinSize->maximum() - ui->spinSize->value() - value);
    ui->spinSize->setValue(ui->spinSize->maximum() - value - ui->spinSpaceAfter->value());

    ui->spinSpaceAfter->blockSignals(false);
    ui->spinSize->blockSignals(false);
}



void PartitionCreateDialog::spinSpaceAfter_valueChanged(int value) {
    ui->spinSpaceBefore->blockSignals(true);
    ui->spinSize->blockSignals(true);

    ui->spinSpaceBefore->setValue(ui->spinSize->maximum() - ui->spinSize->value() - value);
    ui->spinSize->setValue(ui->spinSize->maximum() - ui->spinSpaceBefore->value() - value);

    ui->spinSpaceBefore->blockSignals(false);
    ui->spinSize->blockSignals(false);
}



void PartitionCreateDialog::spinSize_valueChanged(int value) {
    ui->spinSpaceBefore->blockSignals(true);
    ui->spinSpaceAfter->blockSignals(true);

    ui->spinSpaceAfter->setValue(ui->spinSize->maximum() - ui->spinSpaceBefore->value() - value);
    ui->spinSpaceBefore->setValue(ui->spinSize->maximum() - value - ui->spinSpaceAfter->value());

    ui->spinSpaceBefore->blockSignals(false);
    ui->spinSpaceAfter->blockSignals(false);
}



void PartitionCreateDialog::buttonBox_accepted() {
    MParted::PartitionAlignment alignment = MParted::ALIGN_MEBIBYTE;
    bool asExtended;

    // Find alignment
    if (ui->comboBoxAlignment->currentIndex() == 1)
        alignment = MParted::ALIGN_CYLINDER;
    else
        alignment = MParted::ALIGN_MEBIBYTE;

    // Check if extended
    if (ui->comboBoxType->currentIndex() == 1)
        asExtended = true;
    else
        asExtended = false;


    emit requestNewPartition(unallocatedPartition, (unsigned int)ui->spinSpaceBefore->value(), (unsigned int)ui->spinSize->value(), getCurrentFilesystem(), asExtended, alignment, ui->textBoxLabel->text().trimmed());
    this->close();
}



void PartitionCreateDialog::comboBoxType_indexChanged(int index) {
    if (index == 1) {
        ui->comboBoxSupportedFilesystems->clear();
        ui->comboBoxSupportedFilesystems->setEnabled(false);
        ui->textBoxLabel->clear();
        ui->textBoxLabel->setEnabled(false);
    }
    else {
        ui->comboBoxSupportedFilesystems->setEnabled(true);
        ui->textBoxLabel->setEnabled(true);
        fillFilesystemComboBox();
    }
}
