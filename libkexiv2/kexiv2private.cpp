/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Private methods.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// Local includes.

#include "kexiv2private.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

std::string KExiv2::commentsMetaData() const
{
    return d->imageComments;
}

Exiv2::ExifData KExiv2::exifMetaData() const
{
    return d->exifMetadata;
}

Exiv2::IptcData KExiv2::iptcMetaData() const
{
    return d->iptcMetadata;
}

Exiv2::XmpData KExiv2::xmpMetaData() const
{
#ifdef _XMP_SUPPORT_
    return d->xmpMetadata;
#else
    return Exiv2::XmpData();
#endif
}

void KExiv2::setComments(std::string comments)
{
    d->imageComments = comments;
}

void KExiv2::setExif(Exiv2::ExifData data)
{
    d->exifMetadata = data;
}

void KExiv2::setIptc(Exiv2::IptcData data)
{
    d->iptcMetadata = data;
}

void KExiv2::setXmp(Exiv2::XmpData data)
{
#ifdef _XMP_SUPPORT_
    d->xmpMetadata = data;
#endif
}

}  // NameSpace KExiv2Iface
