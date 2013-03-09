/*
 *  Manjaro Parted Backend
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

#ifndef OPERATIONFORMAT_H
#define OPERATIONFORMAT_H

#include <QString>
#include "operation.h"


namespace MParted {


class OperationFormat : public Operation
{
public:
    OperationFormat(Device & device, Partition & partition_orig, Partition & partition_new);

    bool applyToVisual(Partitions & partitions);
    QString createDescription();

};


}

#endif // OPERATIONFORMAT_H
