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
#include <QIcon>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QListWidgetItem>
#include "const.h"


namespace Ui {
class Page_UserImage;
}


class Page_UserImage : public MaWizPage
{
    Q_OBJECT
    
public:
    explicit Page_UserImage();
    ~Page_UserImage();

    void init();
    bool ready();
    
private:
    Ui::Page_UserImage *ui;
    bool isReady;

protected slots:
    void currentItemChanged(QListWidgetItem *item);

};

#endif // PAGE_USERIMAGE_H
