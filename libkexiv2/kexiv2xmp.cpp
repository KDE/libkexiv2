/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

//-- Xmp manipulation methods --------------------------------

bool KExiv2::asXmp()
{
#ifdef _XMP_SUPPORT_
    return !d->xmpMetadata.empty();
#else
    return false;
#endif
}

bool KExiv2::clearXmp()
{
#ifdef _XMP_SUPPORT_
    try
    {
        d->xmpMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot clear Xmp data using Exiv2 ", e);
    }
#endif

    return false;
}

QByteArray KExiv2::getXmp() const
{
#ifdef _XMP_SUPPORT_
    try
    {
        if (!d->xmpMetadata.empty())
        {

            std::string xmpPacket;
            Exiv2::XmpParser::encode(xmpPacket, d->xmpMetadata);
            QByteArray data(xmpPacket.data(), xmpPacket.size());
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot get Xmp data using Exiv2 ", e);
    }
#endif

    return QByteArray();
}

bool KExiv2::setXmp(const QByteArray& data)
{
#ifdef _XMP_SUPPORT_
    try
    {
        if (!data.isEmpty())
        {

            std::string xmpPacket;
            xmpPacket.assign(data.data(), data.size());
            if (Exiv2::XmpParser::decode(d->xmpMetadata, xmpPacket) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Xmp data using Exiv2 ", e);
    }
#endif

    return false;
}

}  // NameSpace KExiv2Iface
