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

#include "partresizerwidget.h"


PartResizerWidget::PartResizerWidget(QWidget *parent) :
    QWidget(parent),
    partWidget(this),
    leftHandle(this),
    rightHandle(this)
{
    setMinimumHeight(handleHeight());
    // set margins to accommodate to top/bottom button asymmetric layouts
    QStyleOptionButton bOpt;
    bOpt.initFrom(this);

    QRect buttonRect(style()->subElementRect(QStyle::SE_PushButtonContents, &bOpt));

    int asym = (rect().bottom() - buttonRect.bottom()) - (buttonRect.top() - rect().top());
    if (asym > 0)
        setContentsMargins(0, asym, 0, 0);
    else
        setContentsMargins(0, 0, 0, asym);

    QPixmap pixmap(handleWidth(), handleHeight());
    pixmap.fill(Qt::transparent);
    {
        QPainter p(&pixmap);
        QStyleOption opt;
        opt.state |= QStyle::State_Horizontal;
        opt.rect = pixmap.rect().adjusted(0, 2, 0, -2);
        style()->drawControl(QStyle::CE_Splitter, &opt, &p, this);
    }

    leftHandle.setPixmap(pixmap);
    rightHandle.setPixmap(pixmap);
    leftHandle.setFixedSize(handleWidth(), handleHeight());
    rightHandle.setFixedSize(handleWidth(), handleHeight());


    //if (!readOnly()) {
    leftHandle.setCursor(Qt::SizeHorCursor);
    rightHandle.setCursor(Qt::SizeHorCursor);
    //}
}



qint32 PartResizerWidget::handleWidth() const {
    return style()->pixelMetric(QStyle::PM_SplitterWidth);
}



qint32 PartResizerWidget::handleHeight() const {
    return 59;
}



qint64 PartResizerWidget::sectorsPerPixel() const {
    return partition.getSectorLength() / (width() - 2 * handleWidth());
}



int PartResizerWidget::partWidgetStart() const {
    return handleWidth() + (partition.sector_start - 1) / sectorsPerPixel();
}



int PartResizerWidget::partWidgetWidth() const {
    return partition.getSectorLength() / sectorsPerPixel();
}




void PartResizerWidget::updatePositions() {
    QMargins margins(contentsMargins());

    partWidget.move(partWidgetStart() + margins.left(), margins.top());
    partWidget.resize(partWidgetWidth() - margins.left() - margins.right(), height() - margins.top() - margins.bottom());

    leftHandle.move(partWidgetStart() - leftHandle.width(), 0);

    rightHandle.move(partWidgetStart() + partWidgetWidth(), 0);

    partWidget.update();
}



void PartResizerWidget::paintEvent(QPaintEvent*) {
    // draw sunken frame
    QPainter painter(this);
    QStyleOptionFrameV3 opt;
    opt.initFrom(this);
    opt.frameShape = QFrame::StyledPanel;
    opt.state |= QStyle::State_Sunken;

    // disable mouse over and focus state
    opt.state &= ~QStyle::State_MouseOver;
    opt.state &= ~QStyle::State_HasFocus;
    opt.rect.adjust(handleWidth(), 0, -handleWidth(), 0);
    style()->drawControl(QStyle::CE_ShapedFrame, &opt, &painter, this);
}
