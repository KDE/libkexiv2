/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2009-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KEXIV2DATA_H
#define KEXIV2DATA_H

// QT includes.

#include <QtCore/QSharedDataPointer>

// Local includes.

#include "libkexiv2_export.h"

namespace KExiv2Iface
{

class KExiv2DataPriv;

class KEXIV2_EXPORT KExiv2Data
{
public:

    KExiv2Data();
    KExiv2Data(const KExiv2Data&);
    ~KExiv2Data();
    KExiv2Data& operator=(const KExiv2Data&);

private:

    friend class KExiv2;
    QSharedDataPointer<KExiv2DataPriv> d;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2_H */
