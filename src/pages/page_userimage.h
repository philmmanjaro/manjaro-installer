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

#ifndef PAGE_USERIMAGE_H
#define PAGE_USERIMAGE_H

#include "MaWiz/mawizpage.h"
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QFileDialog>
#include "const.h"


namespace Ui {
class Page_UserImage;
}


class UserImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserImageWidget();
    void setImage(QImage image);

signals:
    void clicked();

protected:
    QImage image, background, notification;

    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};



class Page_UserImage : public MaWizPage
{
    Q_OBJECT
    
public:
    explicit Page_UserImage();
    ~Page_UserImage();

    virtual void init() {}
    
private:
    Ui::Page_UserImage *ui;
    UserImageWidget imageWidget;

protected slots:
    void imageWidget_clicked();

};

#endif // PAGE_USERIMAGE_H
