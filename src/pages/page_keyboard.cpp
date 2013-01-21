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


#include "page_keyboard.h"
#include "ui_page_keyboard.h"



Page_Keyboard::Page_Keyboard() :
    MaWizPage(),
    ui(new Ui::Page_Keyboard)
{
    ui->setupUi(this);
    setTitle(tr("Select your Keyboard Layout"));
    setHelpURL("http://wiki.manjaro.org");
}



Page_Keyboard::~Page_Keyboard()
{
    delete ui;
}
