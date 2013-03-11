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

#include "minst.h"


MInst::MInst() :
    MaWiz()
{
    // Setup widget
    setWindowTitle(tr("Manjaro Installation"));
    setButtonQuitText(tr("Quit"));
    setButtonBackText(tr("Back"));
    setButtonForwardText(tr("Forward"));
    setButtonHelpText(tr("Help"));
    setButtonReturnText(tr("Return"));
    setHelpTitle(tr("Help"));

    addPage(&pageIntro);
    addPage(&pageManagePartitions);
    addPage(&pageGPUDriver);
    addPage(&pageTimeZone);
    addPage(&pageKeyboard);

    // Connect Signals and Slots
    connect(this, SIGNAL(quitRequested())   ,   qApp, SLOT(quit()));
}


MInst::~MInst()
{
}



void MInst::init() {
    // Init pages
    pageIntro.init();
    pageManagePartitions.init();
    pageGPUDriver.init();
    pageTimeZone.init();
    pageKeyboard.init();

    // Show widget
    show();
}
