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

#ifndef PAGE_TIMEZONE_H
#define PAGE_TIMEZONE_H

#include <MaWiz/mawizpage.h>
#include "widgets/timezonewidget.h"
#include "global.h"


namespace Ui {
class Page_TimeZone;
}


class Page_TimeZone : public MaWizPage
{
    Q_OBJECT
    
public:
    explicit Page_TimeZone();
    ~Page_TimeZone();

    void init();
    
private:
    Ui::Page_TimeZone *ui;
    TimeZoneWidget timeZoneWidget;
    bool blockTimeZoneWidgetSet;

private slots:
    void comboRegionIndexChanged(const QString &text);
    void comboZoneIndexChanged(const QString &text);
    void timeZoneLocationChanged(Global::Location location);

};

#endif // PAGE_TIMEZONE_H
