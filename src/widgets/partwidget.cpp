#include "partwidget.h"

PartWidget::PartWidget(QWidget *parent) :
    QWidget(parent)
{
    QStyle* style = new QPlastiqueStyle();
    style->setParent(this);
    setStyle(style);
}



void PartWidget::setPartition(MParted::Partition & partition) {
    this->partition = partition;
}



void PartWidget::paintEvent(QPaintEvent*)
{
    const int usedPercentage = partition.getSectorsUsed() * 100 / partition.getSectorLength();
    const int w = width() * usedPercentage / 100;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    const QColor base = partition.getFilesystemColor();

    /*if (!partition()->roles().has(PartitionRole::Unallocated))
    {*/
    // TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        const QColor dark = base.darker(105);
        const QColor light = base.lighter(120);

        // draw free space background
        drawGradient(&painter, light, QRect(0, 0, width(), height()));

        // draw used space in front of that
        drawGradient(&painter, dark, QRect(0, 0, w, height() - 1));
   /* }
    else
        drawGradient(&painter, base, QRect(0, 0, width(), height()), isActive());*/

    // draw name and size
    QString text = QString(partition.path).remove("/dev/") + '\n' + partition.getFormattedSize();

    const QRect textRect(0, 0, width() - 1, height() - 1);
    const QRect boundingRect = painter.boundingRect(textRect, Qt::AlignVCenter | Qt::AlignHCenter, text);
    if (boundingRect.x() > 3 && boundingRect.y() > 3) {
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignHCenter, text);
    }
}



void PartWidget::drawGradient(QPainter* painter, const QColor& color, const QRect& rect, bool active) const
{
    if (rect.width() < 8)
        return;

    QStyleOptionButton option;
    option.initFrom(this);
    option.rect = rect;
    option.palette.setColor(QPalette::Button, color);
    option.palette.setColor(QPalette::Window, color);
    option.state |= QStyle::State_Raised;
    if (!active)
        option.state &= ~QStyle::State_MouseOver;
    else
        option.state |= QStyle::State_MouseOver;

    style()->drawControl(QStyle::CE_PushButtonBevel, &option, painter, this);
}
