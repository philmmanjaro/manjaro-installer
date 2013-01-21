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

#ifndef MINST_H
#define MINST_H

#include <MaWiz/mawiz.h>
#include <MaWiz/mawizpage.h>
#include <QApplication>
#include <QString>
#include "pages/page_intro.h"
#include "pages/page_timezone.h"
#include "pages/page_managepartitions.h"
#include "pages/page_gpudriver.h"
#include "pages/page_keyboard.h"


class MInst : public MaWiz
{
    Q_OBJECT
    
public:
    explicit MInst();
    ~MInst();

public slots:
    void init();

private:
    Page_Intro pageIntro;
    Page_TimeZone pageTimeZone;
    Page_ManagePartitions pageManagePartitions;
    Page_GPUDriver pageGPUDriver;
    Page_Keyboard pageKeyboard;

};

#endif // MINST_H
