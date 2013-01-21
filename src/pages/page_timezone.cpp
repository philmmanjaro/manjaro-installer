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

#include "page_timezone.h"
#include "ui_page_timezone.h"



Page_TimeZone::Page_TimeZone() :
    MaWizPage(),
    ui(new Ui::Page_TimeZone)
{
    ui->setupUi(this);
    setTitle(tr("Select your Time Zone"));
    setHelpURL("http://wiki.manjaro.org");
    blockTimeZoneWidgetSet = false;

    // Setup time zone widget
    ui->mapLayout->insertWidget(0, &timeZoneWidget);

    // Connect signals and slots
    connect(ui->comboRegion, SIGNAL(currentIndexChanged(QString))   ,   this, SLOT(comboRegionIndexChanged(QString)));
    connect(ui->comboZone, SIGNAL(currentIndexChanged(QString))   ,   this, SLOT(comboZoneIndexChanged(QString)));
    connect(&timeZoneWidget, SIGNAL(locationChanged(Global::Location))  ,   this, SLOT(timeZoneLocationChanged(Global::Location)));
}



Page_TimeZone::~Page_TimeZone()
{
    delete ui;
}



void Page_TimeZone::init() {
    // Block signals
    ui->comboRegion->blockSignals(true);
    ui->comboZone->blockSignals(true);

    // Setup locations
    QHash<QString, QList<Global::Location> > regions = Global::getLocations();

    QStringList keys = regions.keys();
    keys.sort();

    for (int i = 0; i < keys.size(); ++i) {
        ui->comboRegion->addItem(keys.at(i));
    }

    // Enable signals
    ui->comboRegion->blockSignals(false);
    ui->comboZone->blockSignals(false);

    // Fill zone combobox
    comboRegionIndexChanged(ui->comboRegion->currentText());
}



//###
//### Private
//###



void Page_TimeZone::comboRegionIndexChanged(const QString &text) {
    QHash<QString, QList<Global::Location> > regions = Global::getLocations();
    if (!regions.contains(text))
        return;

    // Block signals
    ui->comboZone->blockSignals(true);

    // Fill combo with zones
    ui->comboZone->clear();

    QList<Global::Location> zones = regions.value(text);
    for (int i = 0; i < zones.size(); ++i) {
        ui->comboZone->addItem(zones.at(i).zone);
    }

    ui->comboZone->model()->sort(0);

    // Enable signals
    ui->comboZone->blockSignals(false);

    // Trigger signal
    comboZoneIndexChanged(ui->comboZone->currentText());
}



void Page_TimeZone::comboZoneIndexChanged(const QString &text) {
    if (!blockTimeZoneWidgetSet)
        timeZoneWidget.setCurrentLocation(ui->comboRegion->currentText(), text);
}



void Page_TimeZone::timeZoneLocationChanged(Global::Location location) {
    // Block signals
    blockTimeZoneWidgetSet = true;

    // Set region index
    int index = ui->comboRegion->findText(location.region);
    if (index < 0)
        return;

    ui->comboRegion->setCurrentIndex(index);

    // Set zone index
    index = ui->comboZone->findText(location.zone);
    if (index < 0)
        return;

    ui->comboZone->setCurrentIndex(index);

    // Enable signals
    blockTimeZoneWidgetSet = false;
}
