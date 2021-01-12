/*
    SPDX-FileCopyrightText: 2009-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2009-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KEXIV2DATA_H
#define KEXIV2DATA_H

// Qt includes

#include <QSharedDataPointer>

// Local includes

#include "libkexiv2_export.h"

namespace KExiv2Iface
{

class LIBKEXIV2_EXPORT KExiv2Data
{
public:

    KExiv2Data();
    KExiv2Data(const KExiv2Data&);
    ~KExiv2Data();

    KExiv2Data& operator=(const KExiv2Data&);

private:

    QSharedDataPointer<class KExiv2DataPrivate> d;

    friend class KExiv2;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2DATA_H */
