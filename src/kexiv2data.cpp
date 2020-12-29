/*
    SPDX-FileCopyrightText: 2009-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2009-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Local includes

#include "kexiv2data.h"
#include "kexiv2.h"
#include "kexiv2_p.h"

namespace KExiv2Iface
{

KExiv2Data::KExiv2Data()
    : d(nullptr)
{
}

KExiv2Data::KExiv2Data(const KExiv2Data& other)
{
    d = other.d;
}

KExiv2Data::~KExiv2Data()
{
}

KExiv2Data& KExiv2Data::operator=(const KExiv2Data& other)
{
    d = other.d;
    return *this;
}

}  // NameSpace KExiv2Iface
