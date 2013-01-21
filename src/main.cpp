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
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QThread>
#include "global.h"


class Preload : public QThread
{
public:
    void run() {
        Global::init();
    }
};



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Preload preload;
    MInst wizard;

    // Show splash screen
    QSplashScreen splash(QPixmap(":/images/resources/splash.png"));
    splash.show();

    // Show wizard window and hide splash screen when preload finished
    QObject::connect(&preload, SIGNAL(finished())   ,   &wizard, SLOT(init()));
    QObject::connect(&preload, SIGNAL(finished())   ,   &splash, SLOT(close()));

    // Start preload process
    preload.start();

    return app.exec();
}
