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

    // Keyboard Preview
    ui->KBPreviewLayout->addWidget(&keyboardPreview);

    // Connect signals and slots
    connect(ui->listLayout, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))   ,   this, SLOT(listLayout_currentItemChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->listVariant, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))   ,   this, SLOT(listVariant_currentItemChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->buttonRestore, SIGNAL(clicked())  ,   this, SLOT(buttonRestore_clicked()));
    connect(ui->comboBoxModel, SIGNAL(currentIndexChanged(const QString))   ,   this, SLOT(comboBoxModel_currentIndexChanged(QString)));
    connect(ui->listVariant, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))   ,  this, SIGNAL(checkReady()));
}



Page_Keyboard::~Page_Keyboard()
{
    delete ui;
}



void Page_Keyboard::activated() {
    // Default focus
    ui->listLayout->setFocus();
}



void Page_Keyboard::init() {
    //### Detect current keyboard layout and variant
    QString currentLayout;
    QProcess process;
    process.start("setxkbmap", QStringList() << "-print");

    if (process.waitForFinished()) {
        QStringList list = QString(process.readAll()).split("\n", QString::SkipEmptyParts);

        foreach(QString line, list) {
            line = line.trimmed();
            if (!line.startsWith("xkb_symbols"))
                continue;

            line = line.remove("}").remove("{").remove(";");
            line = line.mid(line.indexOf("\"") + 1);

            QStringList split = line.split("+", QString::SkipEmptyParts);
            if (split.size() >= 2) {
                currentLayout = split.at(1);

                if (currentLayout.contains("("))
                    currentLayout = currentLayout.mid(0, currentLayout.indexOf("(")).trimmed();
            }
        }
    }



    //### Models
    models = Global::getKeyboardModels();
    QMapIterator<QString, QString> mi(models);

    ui->comboBoxModel->blockSignals(true);

    while (mi.hasNext()) {
        mi.next();

        if (mi.value() == "pc105")
            defaultIndex = ui->comboBoxModel->count();

        ui->comboBoxModel->addItem(mi.key());
    }

    ui->comboBoxModel->blockSignals(false);

    // Set to default value pc105
    ui->comboBoxModel->setCurrentIndex(defaultIndex);




    //### Layouts and Variants

    // Block signals
    ui->listLayout->blockSignals(true);

    QMap<QString, Global::KeyboardInfo> layouts = Global::getKeyboardLayouts();
    QMapIterator<QString, Global::KeyboardInfo> li(layouts);
    LayoutItem *currentLayoutItem = NULL;

    while (li.hasNext()) {
        li.next();

        LayoutItem *item = new LayoutItem();
        Global::KeyboardInfo info = li.value();

        item->setText(info.description);
        item->data = li.key();
        item->info = info;
        ui->listLayout->addItem(item);

        // Find current layout index
        if (li.key() == currentLayout)
            currentLayoutItem = item;
    }

    ui->listLayout->sortItems();

    // Unblock signals
    ui->listLayout->blockSignals(false);

    // Set current layout
    if (currentLayoutItem)
        ui->listLayout->setCurrentItem(currentLayoutItem);
}



bool Page_Keyboard::ready() {
    return (ui->listVariant->selectedItems().size() > 0);
}



//###
//### Slots
//###



void Page_Keyboard::listLayout_currentItemChanged(QListWidgetItem * current, QListWidgetItem *) {
    LayoutItem *item = dynamic_cast<LayoutItem*>(current);
    if (!item)
        return;

    // Block signals
    ui->listVariant->blockSignals(true);

    QMap< QString, QString > variants = item->info.variants;
    QMapIterator<QString, QString> li(variants);
    LayoutItem *defaultItem = NULL;

    ui->listVariant->clear();

    while (li.hasNext()) {
        li.next();

        item = new LayoutItem();
        item->setText(li.key());
        item->data = li.value();
        ui->listVariant->addItem(item);

        if (li.value() == "")
            defaultItem = item;
    }

    // Unblock signals
    ui->listVariant->blockSignals(false);

    // Set to default value
    if (defaultItem)
        ui->listVariant->setCurrentItem(defaultItem);
}



void Page_Keyboard::listVariant_currentItemChanged(QListWidgetItem * current, QListWidgetItem *) {
    LayoutItem *layoutItem = dynamic_cast<LayoutItem*>(ui->listLayout->currentItem());
    LayoutItem *variantItem = dynamic_cast<LayoutItem*>(current);

    if (!layoutItem || !variantItem)
        return;

    QString layout = layoutItem->data;
    QString variant = variantItem->data;

    keyboardPreview.setLayout(layout);
    keyboardPreview.setVariant(variant);

    // Set Xorg keyboard layout
    system(QString("setxkbmap -layout \"%1\" -variant \"%2\"").arg(layout, variant).toUtf8());
}



void Page_Keyboard::buttonRestore_clicked() {
    // Set to default value pc105
    ui->comboBoxModel->setCurrentIndex(defaultIndex);
}



void Page_Keyboard::comboBoxModel_currentIndexChanged(const QString & text) {
    QString model = models.value(text, "pc105");

    // Set Xorg keyboard model
    system(QString("setxkbmap -model \"%1\"").arg(model).toUtf8());
}
