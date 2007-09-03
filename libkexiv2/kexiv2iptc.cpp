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

//-- Iptc manipulation methods --------------------------------

bool KExiv2::asIptc()
{
    return !d->iptcMetadata.empty();
}

bool KExiv2::clearIptc()
{
    try
    {
        d->iptcMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot clear Iptc data using Exiv2 ", e);
    }

    return false;
}

QByteArray KExiv2::getIptc(bool addIrbHeader) const
{
    try
    {
        if (!d->iptcMetadata.empty())
        {
            Exiv2::IptcData& iptc = d->iptcMetadata;
            Exiv2::DataBuf c2;

            if (addIrbHeader) 
            {
#if (EXIV2_TEST_VERSION(0,10,0))
                c2 = Exiv2::Photoshop::setIptcIrb(0, 0, iptc);
#else
                qDebug("Exiv2 version is to old. Cannot add Irb header to IPTC metadata");
                return QByteArray();
#endif
            }
            else 
                c2 = iptc.copy();

            QByteArray data((const char*)c2.pData_, c2.size_);
            return data;

        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot get Iptc data using Exiv2 ",e);
    }

    return QByteArray();
}

bool KExiv2::setIptc(const QByteArray& data)
{
    try
    {
        if (!data.isEmpty())
        {
            if (d->iptcMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Iptc data using Exiv2 ", e);
    }

    return false;
}

}  // NameSpace KExiv2Iface
