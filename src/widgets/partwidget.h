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

#ifndef PARTWIDGET_H
#define PARTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QStyle>
#include <QPlastiqueStyle>
#include <QStyleOptionButton>
#include "partitionmanager/mparted.h"

// Based on KDEPartitionManager


class PartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartWidget(QWidget *parent = 0);
    
    void setPartition(MParted::Partition & partition);

protected:
    void paintEvent(QPaintEvent* event);

    void drawGradient(QPainter* painter, const QColor& color, const QRect& rect, bool active = false) const;

private:
    MParted::Partition partition;
    
};

#endif // PARTWIDGET_H
