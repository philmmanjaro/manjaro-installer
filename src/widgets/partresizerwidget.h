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

#ifndef PARTRESIZERWIDGET_H
#define PARTRESIZERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QStyleOptionFrameV3>
#include <QFrame>
#include <QStyle>
#include <QLabel>
#include <QPixmap>
#include <QStyleOptionButton>
#include "partitionmanager/mparted.h"
#include "partwidget.h"

// Based on KDEPartitionManager


class PartResizerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartResizerWidget(QWidget *parent = 0);

    void setPartition(MParted::Partition & partition) { this->partition = partition; partWidget.setPartition(partition); updatePositions();}
    
private:
    MParted::Partition partition;
    PartWidget partWidget;
    QLabel leftHandle;
    QLabel rightHandle;

    qint32 handleWidth() const;
    qint32 handleHeight() const;
    qint64 sectorsPerPixel() const;
    int partWidgetStart() const;
    int partWidgetWidth() const;

    void updatePositions();
    void paintEvent(QPaintEvent* event);
    
};

#endif // PARTRESIZERWIDGET_H
