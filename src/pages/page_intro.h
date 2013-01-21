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

#ifndef PAGE_INTRO_H
#define PAGE_INTRO_H

#include <MaWiz/mawizpage.h>
#include <QString>
#include <QStringList>
#include <QList>
#include <QListWidgetItem>
#include "global.h"


namespace Ui {
class Page_Intro;
}


class Page_Intro : public MaWizPage
{
    Q_OBJECT
    
public:
    explicit Page_Intro();
    ~Page_Intro();

    void init();
    bool ready();
    
private:
    Ui::Page_Intro *ui;
    QHash<QString, QHash<QString, QList<Global::Locale> > > locales;
    QHash<QString, QList<Global::Locale> > currentTerritories;
    QList<Global::Locale> currentLocales;

private slots:
    void listWidgetLanguageItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void listWidgetTerritoryItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void comboBoxLocaleIndexChanged(const QString &text);

};

#endif // PAGE_INTRO_H
