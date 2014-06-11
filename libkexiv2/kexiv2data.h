/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-15
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2009-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2009-2012 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KEXIV2DATA_H
#define KEXIV2DATA_H

// QT includes

#include <QtCore/QSharedDataPointer>

// Local includes

#include "libkexiv2_export.h"

namespace KExiv2Iface
{

class KEXIV2_EXPORT KExiv2Data
{
public:

    KExiv2Data();
    KExiv2Data(const KExiv2Data&);
    ~KExiv2Data();

    KExiv2Data& operator=(const KExiv2Data&);

public:

    // Declared as public due to use in KExiv2Priv class
    class Private;

private:

    QSharedDataPointer<Private> d;

    friend class KExiv2;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2_H */
