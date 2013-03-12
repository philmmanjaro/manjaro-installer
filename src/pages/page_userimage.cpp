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

#include "page_userimage.h"
#include "ui_page_userimage.h"


Page_UserImage::Page_UserImage() :
    MaWizPage(),
    ui(new Ui::Page_UserImage)
{
    ui->setupUi(this);
    setTitle(tr("Set User Image"));
    setHelpURL("");

    ui->userImageLayout->addWidget(&imageWidget);

    connect(&imageWidget, SIGNAL(clicked()) ,   this, SLOT(imageWidget_clicked()));
}



Page_UserImage::~Page_UserImage()
{
    delete ui;
}



void Page_UserImage::imageWidget_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Image"), USER_IMAGES_PATH, tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty())
        imageWidget.setImage(QImage(fileName));
}



UserImageWidget::UserImageWidget() : QWidget()
{
    setCursor(Qt::PointingHandCursor);

    background = QImage(":/images/resources/laptop.png");
    notification = QImage(":/images/resources/addUser.png");

    setMaximumSize(background.size());
    setMinimumSize(background.size());
}



void UserImageWidget::setImage(QImage image) {
    this->image = image.scaled(84, 95, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    repaint();
}



void UserImageWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawImage(0, 0, background);
    p.drawImage(331, 82, image);

    if (underMouse())
        p.drawImage(399, 56, notification);
}



void UserImageWidget::mouseReleaseEvent(QMouseEvent *) {
    emit clicked();
}



void UserImageWidget::enterEvent(QEvent *) {
    repaint();
}



void UserImageWidget::leaveEvent(QEvent *) {
    repaint();
}
