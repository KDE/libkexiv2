/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-15
 * @brief  Comments manipulation methods
 *
 * @author Copyright (C) 2006-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2012 by Marcel Wiesweg
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

#include "kexiv2_p.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

bool KExiv2::canWriteComment(const QString& filePath)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdComment);
        return (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    }
    catch( Exiv2::Error& e )
    {
        std::string s(e.what());
        kError() << "Cannot check Comment access mode using Exiv2 (Error #"
                 << e.code() << ": " << s.c_str() << ")";
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::hasComments() const
{
    return !d->imageComments().empty();
}

bool KExiv2::clearComments() const
{
    return setComments(QByteArray());
}

QByteArray KExiv2::getComments() const
{
    return QByteArray(d->imageComments().data(), d->imageComments().size());
}

QString KExiv2::getCommentsDecoded() const
{
    return d->detectEncodingAndDecode(d->imageComments());
}

bool KExiv2::setComments(const QByteArray& data) const
{
    d->imageComments() = std::string(data.data(), data.size());
    return true;
}

QString KExiv2::detectLanguageAlt(const QString& value, QString& lang)
{
    // Ex. from an Xmp tag Xmp.tiff.copyright: "lang="x-default" (c) Gilles Caulier 2007"

    if (value.size() > 6 && value.startsWith(QString("lang=\"")))
    {
        int pos = value.indexOf(QString("\""), 6);
        if (pos != -1)
        {
            lang = value.mid(6, pos-6);
            return (value.mid(pos+2));
        }
    }

    lang.clear();
    return value;
}

}  // NameSpace KExiv2Iface
