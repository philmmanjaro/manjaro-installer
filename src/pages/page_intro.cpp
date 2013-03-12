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

#include "page_intro.h"
#include "ui_page_intro.h"


Page_Intro::Page_Intro() :
    MaWizPage(),
    ui(new Ui::Page_Intro)
{
    ui->setupUi(this);

    setTitle(tr("Preparing to install Manjaro"));
    setHelpURL("file:///home/mango/Projekte/Manjaro/minst/build-debug/mini-slider/slider.html");
    ui->comboBoxLocale->hide();

    // Connect signals and slots
    connect(ui->listWidgetLanguage, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))   ,   this, SLOT(listWidgetLanguageItemChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->listWidgetTerritory, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))   ,   this, SLOT(listWidgetTerritoryItemChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->comboBoxLocale, SIGNAL(currentIndexChanged(QString))    ,   this, SLOT(comboBoxLocaleIndexChanged(QString)));

    // Ready check signals
    connect(ui->listWidgetLanguage, SIGNAL(itemSelectionChanged())   ,   this, SIGNAL(checkReady()));
    connect(ui->listWidgetTerritory, SIGNAL(itemSelectionChanged())   ,   this, SIGNAL(checkReady()));
}



Page_Intro::~Page_Intro()
{
    delete ui;
}



void Page_Intro::init() {
    // Block signals
    ui->listWidgetLanguage->setEnabled(false);
    ui->listWidgetLanguage->blockSignals(true);
    ui->listWidgetTerritory->blockSignals(true);
    ui->comboBoxLocale->blockSignals(true);


    // Setup locales
    locales = Global::getLocales();

    QStringList keys = locales.keys();
    keys.sort();

    for (int i = 0; i < keys.size(); ++i) {
        ui->listWidgetLanguage->addItem(keys.at(i));
    }


    // Enable signals
    ui->listWidgetLanguage->setEnabled(true);
    ui->listWidgetLanguage->blockSignals(false);
    ui->listWidgetTerritory->blockSignals(false);
    ui->comboBoxLocale->blockSignals(false);
}



bool Page_Intro::ready() {
    return (ui->listWidgetLanguage->selectedItems().size() > 0
            && ui->listWidgetTerritory->selectedItems().size() > 0
            && ui->comboBoxLocale->count() > 0);
}




//###
//### Private
//###


void Page_Intro::listWidgetLanguageItemChanged(QListWidgetItem *current, QListWidgetItem*) {
    // Block signals
    ui->comboBoxLocale->blockSignals(true);
    ui->listWidgetTerritory->blockSignals(true);


    // Clear fields first
    ui->listWidgetTerritory->clear();
    ui->labelDescription->clear();
    ui->comboBoxLocale->clear();
    ui->comboBoxLocale->hide();
    currentTerritories.clear();


    if (!locales.contains(current->text()))
        return;

    currentTerritories = locales.value(current->text());

    QStringList keys = currentTerritories.keys();
    keys.sort();

    for (int i = 0; i < keys.size(); ++i) {
        ui->listWidgetTerritory->addItem(keys.at(i));
    }


    // Enable signals
    ui->listWidgetTerritory->blockSignals(false);
    ui->comboBoxLocale->blockSignals(false);
}




void Page_Intro::listWidgetTerritoryItemChanged(QListWidgetItem *current, QListWidgetItem*) {
    // Block signals
    ui->comboBoxLocale->blockSignals(true);


    QString currentText = current->text();

    // Clear fields first
    ui->labelDescription->clear();
    ui->comboBoxLocale->clear();
    ui->comboBoxLocale->show();
    currentLocales.clear();

    if (!currentTerritories.contains(currentText))
        return;

    currentLocales = currentTerritories.value(currentText);

    // Find item and set text
    for (int i = 0; i < currentLocales.size(); ++i)
        ui->comboBoxLocale->addItem(currentLocales.at(i).locale);

    // Set description
    comboBoxLocaleIndexChanged(ui->comboBoxLocale->currentText());


    // Enable signals
    ui->comboBoxLocale->blockSignals(false);
}



void Page_Intro::comboBoxLocaleIndexChanged(const QString &text) {
    // Clear fields first
    ui->labelDescription->clear();

    // Set right description
    for (int i = 0; i < currentLocales.size(); ++i) {
        if (currentLocales.at(i).locale != text)
            continue;

        ui->labelDescription->setText(currentLocales.at(i).description);
        break;
    }
}
