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


#include "page_usersetup.h"
#include "ui_page_usersetup.h"

Page_UserSetup::Page_UserSetup() :
    MaWizPage(),
    ui(new Ui::Page_UserSetup)
{
    ui->setupUi(this);
    setTitle(tr("Setup System"));
    setHelpURL("");

    readyUsername = readyHostname = readyPassword = readyRootPassword = false;

    // Connect signals and slots
    connect(ui->textBoxUsername,SIGNAL(textChanged(QString))    ,   this, SLOT(username_textchanged(QString)));
    connect(ui->textBoxHostname, SIGNAL(textChanged(QString))   ,   this, SLOT(hostname_textchanged(QString)));
    connect(ui->textBoxUserPassword, SIGNAL(textChanged(QString))   ,   this, SLOT(password_textchanged(QString)));
    connect(ui->textBoxUserVerifiedPassword, SIGNAL(textChanged(QString))   ,   this, SLOT(password_textchanged(QString)));
    connect(ui->textBoxRootPassword, SIGNAL(textChanged(QString))   ,   this, SLOT(rootpassword_textchanged(QString)));
    connect(ui->textBoxVerifiedRootPassword, SIGNAL(textChanged(QString))   ,   this, SLOT(rootpassword_textchanged(QString)));
}



Page_UserSetup::~Page_UserSetup()
{
    delete ui;
}



bool Page_UserSetup::ready() {
    return readyUsername && readyHostname && readyPassword && readyRootPassword;
}



//###
//### Slots
//###



void Page_UserSetup::username_textchanged(QString text) {
    QRegExp rx("^[a-z][-a-z0-9_]*\\$");
    QRegExpValidator val(rx);
    int pos = -1;

    if (text.isEmpty()) {
        ui->labelUsernameError->clear();
        ui->labelUsername->clear();
        readyUsername = false;
    }
    else if (val.validate(text, pos) == QValidator::Invalid) {
        ui->labelUsername->setPixmap(QPixmap(":/images/resources/invalid.png"));
        --pos;

        if (pos >= 0 && pos < text.size())
            ui->labelUsernameError->setText(tr("Your username contains an invalid character '%1'").arg(text.at(pos)));
        else
            ui->labelUsernameError->setText(tr("Your username contains invalid characters!"));

        readyUsername = false;
    }
    else {
        ui->labelUsername->setPixmap(QPixmap(":/images/resources/valid.png"));
        ui->labelUsernameError->clear();
        readyUsername = true;
    }

    emit checkReady();
}



void Page_UserSetup::hostname_textchanged(QString text) {
    QRegExp rx("^[a-zA-Z][-a-zA-Z0-9_]*\\$");
    QRegExpValidator val(rx);
    int pos = -1;

    if (text.isEmpty()) {
        ui->labelHostnameError->clear();
        ui->labelHostname->clear();
        readyHostname= false;
    }
    else if (val.validate(text, pos) == QValidator::Invalid) {
        ui->labelHostname->setPixmap(QPixmap(":/images/resources/invalid.png"));
        --pos;

        if (pos >= 0 && pos < text.size())
            ui->labelHostnameError->setText(tr("Your username contains an invalid character '%1'").arg(text.at(pos)));
        else
            ui->labelHostnameError->setText(tr("Your username contains invalid characters!"));

        readyHostname= false;
    }
    else {
        ui->labelHostname->setPixmap(QPixmap(":/images/resources/valid.png"));
        ui->labelHostnameError->clear();
        readyHostname = true;
    }

    emit checkReady();
}



void Page_UserSetup::password_textchanged(QString) {
    QString pw1 = ui->textBoxUserPassword->text();
    QString pw2 = ui->textBoxUserVerifiedPassword->text();

    if (pw1.isEmpty() && pw2.isEmpty()) {
        ui->labelUserPasswordError->clear();
        ui->labelUserPassword->clear();
        readyPassword= false;
    }
    else if (pw1 != pw2) {
        ui->labelUserPasswordError->setText(tr("Your passwords do not match!"));
        ui->labelUserPassword->setPixmap(QPixmap(":/images/resources/invalid.png"));
        readyPassword= false;
    }
    else {
        ui->labelUserPasswordError->clear();
        ui->labelUserPassword->setPixmap(QPixmap(":/images/resources/valid.png"));
        readyPassword= true;
    }

    emit checkReady();
}



void Page_UserSetup::rootpassword_textchanged(QString) {
    QString pw1 = ui->textBoxRootPassword->text();
    QString pw2 = ui->textBoxVerifiedRootPassword->text();

    if (pw1.isEmpty() && pw2.isEmpty()) {
        ui->labelRootPasswordError->clear();
        ui->labelRootPassword->clear();
        readyRootPassword= false;
    }
    else if (pw1 != pw2) {
        ui->labelRootPasswordError->setText(tr("Your passwords do not match!"));
        ui->labelRootPassword->setPixmap(QPixmap(":/images/resources/invalid.png"));
        readyRootPassword= false;
    }
    else {
        ui->labelRootPasswordError->clear();
        ui->labelRootPassword->setPixmap(QPixmap(":/images/resources/valid.png"));
        readyRootPassword= true;
    }

    emit checkReady();
}
