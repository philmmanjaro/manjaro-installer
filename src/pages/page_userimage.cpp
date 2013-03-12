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

    isReady = false;

    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))   ,   this, SLOT(currentItemChanged(QListWidgetItem*)));
}



Page_UserImage::~Page_UserImage()
{
    delete ui;
}



void Page_UserImage::init() {
    // Get all images in the specific path
    QStringList images = QDir(USER_IMAGES_PATH).entryList(QStringList() << "*.png" << "*.jpg" << "*.bmp", QDir::Files, QDir::Name);

    foreach (QString file, images) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setIcon(QIcon(QString(USER_IMAGES_PATH) + "/" + file));
    }
}



bool Page_UserImage::ready() {
    return isReady;
}



void Page_UserImage::currentItemChanged(QListWidgetItem *) {
    isReady = true;
    emit checkReady();
}
