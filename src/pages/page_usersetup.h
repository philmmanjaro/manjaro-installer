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


#ifndef PAGE_USERSETUP_H
#define PAGE_USERSETUP_H

#include <MaWiz/mawizpage.h>
#include <QRegExp>
#include <QRegExpValidator>
#include <QLineEdit>
#include <QLabel>


namespace Ui {
class Page_UserSetup;
}



class Page_UserSetup : public MaWizPage
{
    Q_OBJECT
    
public:
    explicit Page_UserSetup();
    ~Page_UserSetup();

    virtual void init() {}
    bool ready();
    
private:
    Ui::Page_UserSetup *ui;
    bool readyUsername, readyHostname, readyPassword, readyRootPassword;

protected slots:
    void username_textchanged(QString);
    void hostname_textchanged(QString);
    void password_textchanged(QString);
    void rootpassword_textchanged(QString);
};

#endif // PAGE_USERSETUP_H
