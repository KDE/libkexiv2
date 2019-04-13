/** ===========================================================
 * @file
 *
 * This file is a part of KDE project
 *
 *
 * @date   2009-11-15
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2009-2015 by Gilles Caulier
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
